module top;
    memory u_mem();
    cache u_cache();
endmodule

module memory;
    // implementation
endmodule

module cache;
    // implementation
endmodule

config bad_config_1;
    default liblist work;
    cell cache liblist work;
endconfig
