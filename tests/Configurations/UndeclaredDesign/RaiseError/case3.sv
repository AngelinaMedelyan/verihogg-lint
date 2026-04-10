module databus;
    // implementation
endmodule

module top;
    databus u_bus();
endmodule

config bad_config_4;
    default liblist work;
    cell databus liblist work;
endconfig
