module alu;
    // implementation
endmodule

module top;
    // 'alu' module is NOT instantiated anywhere
endmodule

config bad_config_3;
    default liblist work;
    cell alu liblist work;
endconfig
