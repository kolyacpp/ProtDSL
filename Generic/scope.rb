require_relative "base"
require_relative "var"

module SimInfra
    class Scope

        include GlobalCounter# used for temp variables IDs
        attr_reader :tree, :vars, :parent
        def initialize(parent); @tree = []; @vars = {}; end
        # resolve allows to convert Ruby Integer constants to Constant instance

        def var(name, type)
            @vars[name] = SimInfra::Var.new(self, name, type) # return var
            instance_eval "def #{name.to_s}(); return @vars[:#{name.to_s}]; end"
            stmt :new_var, [@vars[name]] # returns @vars[name]
        end

        def add_var(name, type); var(name, type); self; end

        def method_missing(name)
            var(name, :i32)
        end

        def resolve_const(what)
            return what if (what.class== Var) or (what.class== Constant) # or other known classes
            return Constant.new(self, "const_#{next_counter}", what) if (what.class== Integer)
        end

        def binOp(a,b, op);
            a = resolve_const(a); b = resolve_const(b)
            # TODO: check constant size <= bitsize(var)
            # assert(a.type== b.type|| a.type == :iconst || b.type== :iconst)

            stmt op, [tmpvar(a.type), a, b]
        end

        # redefine! add & sub will never be the same
        def add(a,b); binOp(a,b, :add); end
        def sub(a,b); binOp(a,b, :sub); end
        def band(a,b); binOp(a,b, :and); end
        def bor(a,b); binOp(a,b, :or); end
        def bxor(a,b); binOp(a,b, :xor); end
        def shl(a,b); binOp(a,b, :shl); end
        def shr(a,b); binOp(a,b, :shr); end
        def sar(a,b); binOp(a,b, :sar); end

        def eq(a,b); binOp(a,b, :eq); end
        def ne(a,b); binOp(a,b, :ne); end
        def lt(a,b); binOp(a,b, :lt); end
        def ge(a,b); binOp(a,b, :ge); end
        def ltu(a,b); binOp(a,b, :ltu); end
        def geu(a,b); binOp(a,b, :geu); end

        def select(cond, a, b)
            cond = resolve_const(cond); a = resolve_const(a); b = resolve_const(b)
            stmt :select, [tmpvar(:i32), cond, a, b]
        end

        def branch(target)
            target = resolve_const(target)
            stmt :branch, [target]
        end

        def write(set, idx, val)
            stmt :write, [set, idx, val]
        end

        def read(set, idx)
            stmt :read, [tmpvar(:i32), set, idx]
        end

        def mem_read(addr, size, sign = :u32)
            addr = resolve_const(addr)
            stmt :mem_read, [tmpvar(:i32), addr, size, sign]
        end

        def mem_write(addr, val, size)
            addr = resolve_const(addr); val = resolve_const(val)
            stmt :mem_write, [addr, val, size]
        end

        def syscall; stmt :syscall, []; end
        def ebreak; stmt :ebreak, []; end
        def fence; stmt :fence, []; end

        private def tmpvar(type); var("_tmp#{next_counter}".to_sym, type); end
        # stmtadds statement into tree and retursoperand[0]
        # which result in near all cases
        def stmt(name, operands, attrs= nil);
            @tree << IrStmt.new(name, operands, attrs); operands[0]
        end
    end
end
