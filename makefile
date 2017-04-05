VISITOR_OBJECTS = \
	visitor/print_visitor.o \
	visitor/interpreter.o \
	visitor/ssa_builder.o \
	visitor/assignment_producer.o \

SSA_OBJECTS = \
	ssa/ssa.o \
	ssa/print_visitor.o \
	ssa/emitter.o \
	ssa/reg_alloc.o \
	ssa/constant_propagation.o \
	ssa/instruction_combiner.o \
	ssa/unused_code.o \
	ssa/type_analysis.o \
	ssa/constant_inliner.o \
	ssa/forward_visitor.o \
	ssa/jmp_threader.o \
	ssa/inliner.o \
	ssa/function_merge.o \
	ssa/dominator_builder.o \
	ssa/loop_invariant.o \
	ssa/common_subexpression.o \

ASMJIT_OBJECTS = \
	asmjit/src/asmjit/base/arch.o \
	asmjit/src/asmjit/base/logging.o \
	asmjit/src/asmjit/base/assembler.o \
	asmjit/src/asmjit/base/operand.o \
	asmjit/src/asmjit/base/codebuilder.o \
	asmjit/src/asmjit/base/osutils.o \
	asmjit/src/asmjit/base/codecompiler.o \
	asmjit/src/asmjit/base/regalloc.o \
	asmjit/src/asmjit/base/codeemitter.o \
	asmjit/src/asmjit/base/runtime.o \
	asmjit/src/asmjit/base/codeholder.o \
	asmjit/src/asmjit/base/string.o \
	asmjit/src/asmjit/base/constpool.o \
	asmjit/src/asmjit/base/utils.o \
	asmjit/src/asmjit/base/cpuinfo.o \
	asmjit/src/asmjit/base/vmem.o \
	asmjit/src/asmjit/base/func.o \
	asmjit/src/asmjit/base/zone.o \
	asmjit/src/asmjit/base/globals.o \
	asmjit/src/asmjit/base/inst.o \
	asmjit/src/asmjit/x86/x86assembler.o \
	asmjit/src/asmjit/x86/x86logging.o \
	asmjit/src/asmjit/x86/x86builder.o \
	asmjit/src/asmjit/x86/x86operand.o \
	asmjit/src/asmjit/x86/x86compiler.o \
	asmjit/src/asmjit/x86/x86operand_regs.o \
	asmjit/src/asmjit/x86/x86inst.o \
	asmjit/src/asmjit/x86/x86regalloc.o \
	asmjit/src/asmjit/x86/x86internal.o \
	asmjit/src/asmjit/x86/x86instimpl.o \

MAIN_OBJECTS = \
	main.o \
	tokenizer.o \
	parser.o \
	runtime_error.o \
	optimize.o \
	firstcompile.o \
	error_compiler.o \
	std.o \
	heap.o \
	stack.o \
	value.o \

DEBUGCXXFLAGS := -O0 -g -DSSA_DIAGNOSTICS -DDISASSEMBLY_DIAGNOSTICS -DOPTIMIZATION_DIAGNOSTICS -DINLINING_DIAGNOSTICS -DFIRST_COMPILE_DIAGNOSTICS
RELEASECXXFLAGS := -O3 -DNDEBUG
PROFILECXXFLAGS := -O3 -g -DNDEBUG
CXXFLAGS := -DASMJIT_EMBED -std=c++11 -fno-omit-frame-pointer ${PROFILECXXFLAGS}

vvn : ${MAIN_OBJECTS} ${VISITOR_OBJECTS} ${ASMJIT_OBJECTS} ${SSA_OBJECTS}
	g++ $^ -o vvn

clean:
	rm visitor/*.o
	rm ssa/*.o
	rm *.o
	rm vvn

cleanasmjit:
	rm asmjit/src/asmjit/base/*.o
	rm asmjit/src/asmjit/x86/*.o
