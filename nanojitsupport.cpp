#include "nanojit-central/nanojit/nanojit.h"

using namespace nanojit;
using namespace std;

/* Allocator SPI implementation. */

void*
nanojit::Allocator::allocChunk(size_t nbytes, bool /*fallible*/)
{
    void *p = malloc(nbytes);
    if (!p)
        exit(1);
    return p;
}

void
nanojit::Allocator::freeChunk(void *p) {
    free(p);
}

void
nanojit::Allocator::postReset() {
}


struct LasmSideExit : public SideExit {
    size_t line;
};


/* LIR SPI implementation */

int
nanojit::StackFilter::getTop(LIns*)
{
    return 0;
}

// We lump everything into a single access region for lirasm.
static const AccSet ACCSET_OTHER = (1 << 0);
static const uint8_t LIRASM_NUM_USED_ACCS = 1;

#if defined NJ_VERBOSE
void
nanojit::LInsPrinter::formatGuard(InsBuf *buf, LIns *ins)
{
    RefBuf b1, b2;
    LasmSideExit *x = (LasmSideExit *)ins->record()->exit;
    VMPI_snprintf(buf->buf, buf->len,
            "%s: %s %s -> line=%ld (GuardID=%03d)",
            formatRef(&b1, ins),
            lirNames[ins->opcode()],
            ins->oprnd1() ? formatRef(&b2, ins->oprnd1()) : "",
            (long)x->line,
            ins->record()->profGuardID);
}

void
nanojit::LInsPrinter::formatGuardXov(InsBuf *buf, LIns *ins)
{
    RefBuf b1, b2, b3;
    LasmSideExit *x = (LasmSideExit *)ins->record()->exit;
    VMPI_snprintf(buf->buf, buf->len,
            "%s = %s %s, %s -> line=%ld (GuardID=%03d)",
            formatRef(&b1, ins),
            lirNames[ins->opcode()],
            formatRef(&b2, ins->oprnd1()),
            formatRef(&b3, ins->oprnd2()),
            (long)x->line,
            ins->record()->profGuardID);
}

const char*
nanojit::LInsPrinter::accNames[] = {
    "o",    // (1 << 0) == ACCSET_OTHER
    "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",   //  1..10 (unused)
    "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",   // 11..20 (unused)
    "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",   // 21..30 (unused)
    "?"                                                 //     31 (unused)
};
#endif
