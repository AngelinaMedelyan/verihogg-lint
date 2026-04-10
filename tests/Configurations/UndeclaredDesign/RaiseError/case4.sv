module top;
    fast_mem u_mem();
endmodule

config bad_config_5;
    default liblist rtl_lib;
    cell fast_mem liblist gate_lib;
endconfig
