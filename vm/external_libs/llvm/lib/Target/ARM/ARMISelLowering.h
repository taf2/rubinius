//===-- ARMISelLowering.h - ARM DAG Lowering Interface ----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that ARM uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#ifndef ARMISELLOWERING_H
#define ARMISELLOWERING_H

#include "ARMSubtarget.h"
#include "llvm/Target/TargetLowering.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include <vector>

namespace llvm {
  class ARMConstantPoolValue;

  namespace ARMISD {
    // ARM Specific DAG Nodes
    enum NodeType {
      // Start the numbering where the builting ops and target ops leave off.
      FIRST_NUMBER = ISD::BUILTIN_OP_END+ARM::INSTRUCTION_LIST_END,

      Wrapper,      // Wrapper - A wrapper node for TargetConstantPool,
                    // TargetExternalSymbol, and TargetGlobalAddress.
      WrapperJT,    // WrapperJT - A wrapper node for TargetJumpTable
      
      CALL,         // Function call.
      CALL_PRED,    // Function call that's predicable.
      CALL_NOLINK,  // Function call with branch not branch-and-link.
      tCALL,        // Thumb function call.
      BRCOND,       // Conditional branch.
      BR_JT,        // Jumptable branch.
      RET_FLAG,     // Return with a flag operand.

      PIC_ADD,      // Add with a PC operand and a PIC label.

      CMP,          // ARM compare instructions.
      CMPNZ,        // ARM compare that uses only N or Z flags.
      CMPFP,        // ARM VFP compare instruction, sets FPSCR.
      CMPFPw0,      // ARM VFP compare against zero instruction, sets FPSCR.
      FMSTAT,       // ARM fmstat instruction.
      CMOV,         // ARM conditional move instructions.
      CNEG,         // ARM conditional negate instructions.
      
      FTOSI,        // FP to sint within a FP register.
      FTOUI,        // FP to uint within a FP register.
      SITOF,        // sint to FP within a FP register.
      UITOF,        // uint to FP within a FP register.

      SRL_FLAG,     // V,Flag = srl_flag X -> srl X, 1 + save carry out.
      SRA_FLAG,     // V,Flag = sra_flag X -> sra X, 1 + save carry out.
      RRX,          // V = RRX X, Flag     -> srl X, 1 + shift in carry flag.
      
      FMRRD,        // double to two gprs.
      FMDRR,         // Two gprs to double.

      THREAD_POINTER
    };
  }

  //===----------------------------------------------------------------------===//
  //  ARMTargetLowering - ARM Implementation of the TargetLowering interface
  
  class ARMTargetLowering : public TargetLowering {
    int VarArgsFrameIndex;            // FrameIndex for start of varargs area.
  public:
    explicit ARMTargetLowering(TargetMachine &TM);

    virtual SDOperand LowerOperation(SDOperand Op, SelectionDAG &DAG);
    virtual SDNode *ExpandOperationResult(SDNode *N, SelectionDAG &DAG);
        
    virtual SDOperand PerformDAGCombine(SDNode *N, DAGCombinerInfo &DCI) const;
    
    virtual const char *getTargetNodeName(unsigned Opcode) const;

    virtual MachineBasicBlock *EmitInstrWithCustomInserter(MachineInstr *MI,
                                                       MachineBasicBlock *MBB);

    /// isLegalAddressingMode - Return true if the addressing mode represented
    /// by AM is legal for this target, for a load/store of the specified type.
    virtual bool isLegalAddressingMode(const AddrMode &AM, const Type *Ty)const;
    
    /// getPreIndexedAddressParts - returns true by value, base pointer and
    /// offset pointer and addressing mode by reference if the node's address
    /// can be legally represented as pre-indexed load / store address.
    virtual bool getPreIndexedAddressParts(SDNode *N, SDOperand &Base,
                                           SDOperand &Offset,
                                           ISD::MemIndexedMode &AM,
                                           SelectionDAG &DAG);

    /// getPostIndexedAddressParts - returns true by value, base pointer and
    /// offset pointer and addressing mode by reference if this node can be
    /// combined with a load / store to form a post-indexed load / store.
    virtual bool getPostIndexedAddressParts(SDNode *N, SDNode *Op,
                                            SDOperand &Base, SDOperand &Offset,
                                            ISD::MemIndexedMode &AM,
                                            SelectionDAG &DAG);

    virtual void computeMaskedBitsForTargetNode(const SDOperand Op,
                                                const APInt &Mask,
                                                APInt &KnownZero, 
                                                APInt &KnownOne,
                                                const SelectionDAG &DAG,
                                                unsigned Depth) const;
    ConstraintType getConstraintType(const std::string &Constraint) const;
    std::pair<unsigned, const TargetRegisterClass*> 
      getRegForInlineAsmConstraint(const std::string &Constraint,
                                   MVT::ValueType VT) const;
    std::vector<unsigned>
    getRegClassForInlineAsmConstraint(const std::string &Constraint,
                                      MVT::ValueType VT) const;

    virtual const ARMSubtarget* getSubtarget() {
      return Subtarget;
    }

  private:
    /// Subtarget - Keep a pointer to the ARMSubtarget around so that we can
    /// make the right decision when generating code for different targets.
    const ARMSubtarget *Subtarget;

    /// ARMPCLabelIndex - Keep track the number of ARM PC labels created.
    ///
    unsigned ARMPCLabelIndex;

    SDOperand LowerCALL(SDOperand Op, SelectionDAG &DAG);
    SDOperand LowerGlobalAddressDarwin(SDOperand Op, SelectionDAG &DAG);
    SDOperand LowerGlobalAddressELF(SDOperand Op, SelectionDAG &DAG);
    SDOperand LowerGlobalTLSAddress(SDOperand Op, SelectionDAG &DAG);
    SDOperand LowerToTLSGeneralDynamicModel(GlobalAddressSDNode *GA,
                                            SelectionDAG &DAG);
    SDOperand LowerToTLSExecModels(GlobalAddressSDNode *GA,
                                   SelectionDAG &DAG);
    SDOperand LowerGLOBAL_OFFSET_TABLE(SDOperand Op, SelectionDAG &DAG);
    SDOperand LowerFORMAL_ARGUMENTS(SDOperand Op, SelectionDAG &DAG);
    SDOperand LowerBR_JT(SDOperand Op, SelectionDAG &DAG);

    SDOperand EmitTargetCodeForMemcpy(SelectionDAG &DAG,
                                      SDOperand Chain,
                                      SDOperand Dst, SDOperand Src,
                                      SDOperand Size, unsigned Align,
                                      bool AlwaysInline,
                                      const Value *DstSV, uint64_t DstSVOff,
                                      const Value *SrcSV, uint64_t SrcSVOff);
  };
}

#endif  // ARMISELLOWERING_H
