module \my-cell ;
    // implementation
endmodule

module top;
    \my-cell u_cell ();
endmodule

config good_config_4;
    design work.top;
    default liblist work;
    cell \my-cell liblist work;
endconfig
