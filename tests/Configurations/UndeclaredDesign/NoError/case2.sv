module memory; endmodule
module cache; endmodule
module buffer; endmodule

module top;
    memory u_mem();
    cache u_cache();
    buffer u_buf();
endmodule

config good_config_3;
    design work.top;
    default liblist work;
    cell memory liblist work;
    cell cache liblist work;
    cell buffer liblist work;
endconfig
