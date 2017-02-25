AST_OBJECTS = \
	ast/addition_expression.o \
	ast/subtraction_expression.o \
	ast/multiplication_expression.o \
	ast/division_expression.o \
	ast/integer_expression.o \
	ast/variable_expression.o \

VISITOR_OBJECTS = \
	visitor/print_visitor.o \
	visitor/interpreter.o \
	visitor/compiler.o \
	visitor/autocompiler.o \
	visitor/location_resolver.o \
	visitor/usage_applier.o \
	visitor/recompiler.o \
	visitor/jumping_compiler.o \
	visitor/ssa_builder.o \

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

NANOJIT_OBJECTS = \
	nanojit-central/Containers.o \
	nanojit-central/NativeX64.o \
	nanojit-central/avmplus.o \
	nanojit-central/Assembler.o \
	nanojit-central/Fragmento.o \
	nanojit-central/RegAlloc.o \
	nanojit-central/CodeAlloc.o \
	nanojit-central/LIR.o \
	nanojit-central/VMPI.o \
	nanojit-central/njconfig.o \
	nanojit-central/Allocator.o \

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
	asmjit/src/asmjit/x86/x86assembler.o \
	asmjit/src/asmjit/x86/x86logging.o \
	asmjit/src/asmjit/x86/x86builder.o \
	asmjit/src/asmjit/x86/x86operand.o \
	asmjit/src/asmjit/x86/x86compiler.o \
	asmjit/src/asmjit/x86/x86operand_regs.o \
	asmjit/src/asmjit/x86/x86inst.o \
	asmjit/src/asmjit/x86/x86regalloc.o \
	asmjit/src/asmjit/x86/x86internal.o \
	asmjit/src/asmjit/x86/x86ssetoavxpass.o \

MAIN_OBJECTS = \
	main.o \
	tokenizer.o \
	parser.o \
	location.o \
	runtime_error.o \
	optimize.o \
	functions.o \
	#nanojitsupport.o \

CXXFLAGS := -DASMJIT_EMBED -std=c++11 -O3 -g
#DEFS := -DHAVE_CONFIG_H -DNANOJIT_CENTRAL

main : ${MAIN_OBJECTS} ${AST_OBJECTS} ${VISITOR_OBJECTS} ${ASMJIT_OBJECTS} ${SSA_OBJECTS}
	g++ $^ -o main

clean:
	rm visitor/*.o
	rm ast/*.o
	rm ssa/*.o
	rm *.o
	rm main
