module memory;
    initial $display("Memory module");
endmodule

module top;
    memory u_mem();
endmodule

config good_config_1;
    design work.top;
    default liblist work;
    cell memory liblist work;
endconfig
