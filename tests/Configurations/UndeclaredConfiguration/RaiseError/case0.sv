config my_test_config; 
    design work.testbench_top; 
    instance testbench_top.f use lib2.golden_model;
    default liblist;
endconfig

module testbench_top;
    my_design f();
endmodule
