#include <bfd.h>
#include "bindpp.h"

extern "C" {

#define GET_SELF() c.arg(0)->as_pointer()->extract<bfd*>()

XS(xs_openr) {
    pl::Ctx c(2);

    pl::Str * fname = c.arg(1)->as_str();
    bfd * abfd;
    if (!(abfd = bfd_openr(fname->to_c(), NULL))) {
        pl::Carp::croak("cannot open file: %s\n", fname->to_c());
    }
    bfd_check_format(abfd, bfd_object);
    pl::Pointer ptr(abfd, "Devel::BFD::File");
    c.ret(&ptr);
}

XS(xs_f_printable_name) {
    pl::Ctx c(1);
    bfd * abfd = GET_SELF();
    pl::Str s(bfd_printable_name(abfd));
    c.ret(&s);
}

XS(xs_f_destroy) {
    pl::Ctx c(1);
    bfd * abfd = GET_SELF();
    bfd_close(abfd);
    c.return_true();
}

XS(xs_f_symtable) {
    pl::Ctx c(1);
    bfd * abfd = GET_SELF();

    if(!(bfd_get_file_flags(abfd) & HAS_SYMS)) {
        pl::Carp::croak("bfd err: %s", bfd_errmsg(bfd_get_error()));
    }

    asymbol * store = bfd_make_empty_symbol(abfd);

    void * minisyms;
    unsigned int size;
    int dyn = 0;
    int symnum;
    if ((symnum = bfd_read_minisymbols(abfd, dyn, &minisyms, &size)) < 0) {
        pl::Carp::croak("Cannot read minisymbols: %s", bfd_errmsg(bfd_get_error()));
    }

    char * p = (char*)minisyms;
    pl::Array res;
    for (int i = 0; i < symnum; i++) {
        asymbol *sym = bfd_minisymbol_to_symbol(abfd, dyn, p, store);

        pl::Hash h;
        h.store("name", bfd_asymbol_name(sym));
        h.store("value", bfd_asymbol_value(sym));
        res.push(h.reference());

        p += size;
    }

    c.ret(&res);
}

XS(boot_Devel__BFD) {
    pl::BootstrapCtx b;

    bfd_init();

    {
        pl::Package pkg("Devel::BFD", __FILE__);
        pkg.add_method("openr", xs_openr);
    }

    {
        pl::Package p("Devel::BFD::File", __FILE__);
        p.add_method("printable_name", xs_f_printable_name);
        p.add_method("symtable", xs_f_symtable);
        p.add_method("DESTROY", xs_f_destroy);
    }
}
}

