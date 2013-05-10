#ifndef __MODINTVARPROP_H
#define __MODINTVARPROP_H

#include "../mozartcore.hh"
#include <gecode/int.hh>
#include "cstsupport/intsupport.hh"
namespace mozart {

namespace builtins {

// Notes:
// - All the constraints are posted in vm->getCurrentSpace()
//   TODO: can I assume that the current space is the same space each
//   variable belongs to?
class ModIntVarProp: public Module {
public:
  ModIntVarProp(): Module("IntVarProp") {}

  class Min: public Builtin<Min> {
  public:
    Min(): Builtin("min") {}

    static void call(VM vm, In x0, In x1, In x2) {
      assert(vm->getCurrentSpace()->hasConstraintSpace());
      GecodeSpace& home = vm->getCurrentSpace()->getCstSpace();
      Gecode::min(home,IntVarLike(x0).intVar(vm),IntVarLike(x1).intVar(vm),IntVarLike(x2).intVar(vm));
    }
  };

  class Min2: public Builtin<Min2> {
  public:
    Min2(): Builtin("min2") {}

    static void call(VM vm, In x, In y) {
      assert(vm->getCurrentSpace()->hasConstraintSpace());
      GecodeSpace& home = vm->getCurrentSpace()->getCstSpace();
      Gecode::min(home,getIntVarArgs(vm,x),IntVarLike(y).intVar(vm));
    }
  };

  class Max: public Builtin<Max> {
  public:
    Max(): Builtin("max") {}
    
    static void call(VM vm, In x0, In x1, In x2) {
      assert(vm->getCurrentSpace()->hasConstraintSpace());
      GecodeSpace& home = vm->getCurrentSpace()->getCstSpace();
      Gecode::max(home,IntVarLike(x0).intVar(vm),IntVarLike(x1).intVar(vm),IntVarLike(x2).intVar(vm));
    }
  };
  
  class Max2: public Builtin<Max2> {
  public:
    Max2(): Builtin("max2") {}
    
    static void call(VM vm, In x, In y) {
      assert(vm->getCurrentSpace()->hasConstraintSpace());
      GecodeSpace& home = vm->getCurrentSpace()->getCstSpace();
      Gecode::max(home,getIntVarArgs(vm,x),IntVarLike(y).intVar(vm));
    }
  };

  class Abs: public Builtin<Abs> {
  public:
    Abs(): Builtin("abs") {}
    
    static void call(VM vm, In x0, In x1) {
      assert(vm->getCurrentSpace()->hasConstraintSpace());
      GecodeSpace& home = vm->getCurrentSpace()->getCstSpace();
      Gecode::abs(home,IntVarLike(x0).intVar(vm),IntVarLike(x1).intVar(vm));
    }
  };

  class Mult: public Builtin<Mult> {
  public:
    Mult(): Builtin("mult") {}
    
    static void call(VM vm, In x0, In x1, In x2) {
      assert(vm->getCurrentSpace()->hasConstraintSpace());
      GecodeSpace& home = vm->getCurrentSpace()->getCstSpace();
      Gecode::mult(home,IntVarLike(x0).intVar(vm),IntVarLike(x1).intVar(vm),IntVarLike(x2).intVar(vm));
    }
  };

  class Sqr: public Builtin<Sqr> {
  public:
    Sqr(): Builtin("sqr") {}
    
    static void call(VM vm, In x0, In x1) {
      assert(vm->getCurrentSpace()->hasConstraintSpace());
      GecodeSpace& home = vm->getCurrentSpace()->getCstSpace();
      Gecode::sqr(home,IntVarLike(x0).intVar(vm),IntVarLike(x1).intVar(vm));
    }
  };

  class Sqrt: public Builtin<Sqrt> {
  public:
    Sqrt(): Builtin("sqrt") {}
    
    static void call(VM vm, In x0, In x1) {
      assert(vm->getCurrentSpace()->hasConstraintSpace());
      GecodeSpace& home = vm->getCurrentSpace()->getCstSpace();
      Gecode::sqrt(home,IntVarLike(x0).intVar(vm),IntVarLike(x1).intVar(vm));
    }
  };
  
  class Divmod: public Builtin<Divmod> {
  public:
    Divmod(): Builtin("divmod") {}
    
    static void call(VM vm, In x0, In x1, In x2, In x3) {
      assert(vm->getCurrentSpace()->hasConstraintSpace());
      GecodeSpace& home = vm->getCurrentSpace()->getCstSpace();
      Gecode::divmod(home,IntVarLike(x0).intVar(vm),IntVarLike(x1).intVar(vm),IntVarLike(x2).intVar(vm),IntVarLike(x3).intVar(vm));
      home.status();
    }
  };

  class DivI: public Builtin<DivI> {
  public:
    DivI(): Builtin("divI") {}
    
    static void call(VM vm, In x0, In x1, In x2) {
      assert(vm->getCurrentSpace()->hasConstraintSpace());
      GecodeSpace& home = vm->getCurrentSpace()->getCstSpace();
      Gecode::div(home,IntVarLike(x0).intVar(vm),IntVarLike(x1).intVar(vm),IntVarLike(x2).intVar(vm), Gecode::ICL_BND);
      home.status();
    }
  };

  class ModI: public Builtin<ModI> {
  public:
    ModI(): Builtin("modI") {}
    
    static void call(VM vm, In x0, In x1, In x2) {
      assert(vm->getCurrentSpace()->hasConstraintSpace());
      GecodeSpace& home = vm->getCurrentSpace()->getCstSpace();
      Gecode::mod(home,IntVarLike(x0).intVar(vm),IntVarLike(x1).intVar(vm),IntVarLike(x2).intVar(vm));
      home.status();
    }
  };

  class Dom2: public Builtin<Dom2> {
  public:
    Dom2(): Builtin("dom2") {}

    static void call(VM vm, In x, In n) {
      assert(vm->getCurrentSpace()->hasConstraintSpace());
      GecodeSpace& home = vm->getCurrentSpace()->getCstSpace();

      if(IntVarLike(x).isIntVarLike(vm) and n.is<SmallInt>()){
	nativeint num = n.as<SmallInt>().value();
	Gecode::dom(home,IntVarLike(x).intVar(vm),(int)(num));
      }else if(isIntVarArgs(vm,x) and n.is<SmallInt>()){
	nativeint num = n.as<SmallInt>().value();
	Gecode::dom(home,getIntVarArgs(vm,x),(int)(num));
      }else if(IntVarLike(x).isIntVarLike(vm) and isIntSet(vm,n)){
	Gecode::dom(home,IntVarLike(x).intVar(vm),getIntSet(vm,n));
      }else if(isIntVarArgs(vm,x) and isIntSet(vm,n)){
	Gecode::dom(home,getIntVarArgs(vm,x),getIntSet(vm,n));
      }else{
	raiseTypeError(vm, MOZART_STR("Propagator posted domain malformed"), x);
      }
    }
  };

  class Dom3: public Builtin<Dom3> {
  public:
    Dom3(): Builtin("dom3") {}

    static void call(VM vm, In x, In n, In m) {
      assert(vm->getCurrentSpace()->hasConstraintSpace());
      GecodeSpace& home = vm->getCurrentSpace()->getCstSpace();
      
      if(IntVarLike(x).isIntVarLike(vm) and n.is<SmallInt>() and m.is<SmallInt>()){
	nativeint min = n.as<SmallInt>().value();
	nativeint max = m.as<SmallInt>().value();
	Gecode::dom(home,IntVarLike(x).intVar(vm),(int)(min),(int)(max));
      }else if(isIntVarArgs(vm,x) and n.is<SmallInt>() and m.is<SmallInt>()){
	nativeint min = n.as<SmallInt>().value();
	nativeint max = m.as<SmallInt>().value();
	Gecode::dom(home,getIntVarArgs(vm,x),(int)(min),(int)(max));
      }else if(IntVarLike(x).isIntVarLike(vm) and n.is<SmallInt>() and BoolVarLike(m).isBoolVarLike(vm)){
	nativeint num = n.as<SmallInt>().value();
	Gecode::dom(home,IntVarLike(x).intVar(vm),(int)(num),BoolVarLike(m).boolVar(vm));
      }else if(IntVarLike(x).isIntVarLike(vm) and isIntSet(vm,n) and BoolVarLike(m).isBoolVarLike(vm)){
	Gecode::dom(home,IntVarLike(x).intVar(vm),getIntSet(vm,n),BoolVarLike(m).boolVar(vm));
      }else{
	raiseTypeError(vm, MOZART_STR("Propagator posted domain malformed"), x);
      }
    }
  };

  class Dom4: public Builtin<Dom4> {
  public:
    Dom4(): Builtin("dom4") {}

    static void call(VM vm, In x, In n, In m, In b) {
      assert(vm->getCurrentSpace()->hasConstraintSpace());
      GecodeSpace& home = vm->getCurrentSpace()->getCstSpace();
      
      if(IntVarLike(x).isIntVarLike(vm) and n.is<SmallInt>() and m.is<SmallInt>() and BoolVarLike(b).isBoolVarLike(vm)){
	nativeint min = n.as<SmallInt>().value();
	nativeint max = m.as<SmallInt>().value();
	Gecode::dom(home,IntVarLike(x).intVar(vm),(int)(min),(int)(max),BoolVarLike(m).boolVar(vm));
      }else{
	raiseTypeError(vm, MOZART_STR("Propagator posted domain malformed"), x);
      }
    }
  };

 class Count: public Builtin<Count> {
  public:
    Count(): Builtin("count") {}

    static void call(VM vm, In x, In n, In r, In m) {
      assert(vm->getCurrentSpace()->hasConstraintSpace());
      GecodeSpace& home = vm->getCurrentSpace()->getCstSpace();
      Gecode::IntRelType rt = atomToRelType(vm,r);
      Gecode::count(home,getIntVarArgs(vm,x),(int)(n.as<SmallInt>().value()),rt,(int)(m.as<SmallInt>().value()));
    }
  };

  class Rel: public Builtin<Rel> {
  public:
    Rel(): Builtin("rel") {}

    static void call(VM vm, In x0, In r, In x1, In icl) {
      assert(vm->getCurrentSpace()->hasConstraintSpace());
      GecodeSpace& home = vm->getCurrentSpace()->getCstSpace();
      Gecode::IntRelType rt = atomToRelType(vm,r);
      Gecode::rel(home,IntVarLike(x0).intVar(vm),rt,IntVarLike(x1).intVar(vm));
    }
  };

  class Distinct: public Builtin<Distinct> {
  public:
    Distinct(): Builtin("distinct") {}
    
    static void call(VM vm, In x) {
      assert(vm->getCurrentSpace()->hasConstraintSpace());
      GecodeSpace& home = vm->getCurrentSpace()->getCstSpace();
      Gecode::distinct(home,getIntVarArgs(vm,x));
    }
  };

  class Distinct2: public Builtin<Distinct2> {
  public:
    Distinct2(): Builtin("distinct2") {}
    
    static void call(VM vm, In v, In x) {
      assert(vm->getCurrentSpace()->hasConstraintSpace());
      GecodeSpace& home = vm->getCurrentSpace()->getCstSpace();
      Gecode::distinct(home,getIntArgs(vm,v),getIntVarArgs(vm,x));
    }
  };

  class Linear: public Builtin<Linear> {
  public:
    Linear(): Builtin("linear") {}
    
    static void call(VM vm, In v, In x, In r, In c) {
      assert(vm->getCurrentSpace()->hasConstraintSpace());
      GecodeSpace& home = vm->getCurrentSpace()->getCstSpace();
      Gecode::IntRelType rt = atomToRelType(vm,r);
      assert(c.is<SmallInt>());
      nativeint val=c.as<SmallInt>().value();
      Gecode::linear(home,getIntArgs(vm,v),getIntVarArgs(vm,x),rt,(int)(val));
    }
  };

  class Branch: public Builtin<Branch> {
  public:
    Branch(): Builtin("branch") {}
    static void call(VM vm, In x, In bvar, In bval) {
      assert(vm->getCurrentSpace()->hasConstraintSpace());
      GecodeSpace& home = vm->getCurrentSpace()->getCstSpace();
      Gecode::IntVarBranch bvart = atomToIntVarBranch(vm,bvar); 
      Gecode::IntValBranch bvalt = atomToIntValBranch(vm,bval);
      Gecode::branch(home,getIntVarArgs(vm,x),bvart,bvalt); 
    }
  };

  class Element: public Builtin<Element>{
  public:
    Element(): Builtin("element") {}
    
    static void call(VM vm, In n, In x0, In x1){
      assert(vm->getCurrentSpace()->hasConstraintSpace());
      GecodeSpace& home = vm->getCurrentSpace()->getCstSpace();
      Gecode::element(home, getIntSharedArray(vm,n), IntVarLike(x0).intVar(vm),IntVarLike(x1).intVar(vm));
    }
  };

}; // class ModIntVarProp
} // namespace builtins
} // namespace mozart
#endif // __MODINTVARPROP_H
