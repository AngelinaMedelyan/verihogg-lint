config cfg1; 
    design work.testbench_top; 
    instance testbench_top.f use lib2.golden_model;
    default liblist;
endconfig

config cfg2; 
    design work.testbench_top; 
    instancge testbench_top.f use lib2.golden_model;
    default liblist;
endconfig
