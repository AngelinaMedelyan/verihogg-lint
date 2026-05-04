module top;
  my_design f();
endmodule // top

module cpu;
  my_design f();
endmodule // cpu

module mem;
  my_design f();
endmodule // mem

config parent_cfg;
  design work.top;
  instance top.cpu use work.cpu_cfg;
  instance top.mem use work.mem_cfg;
endconfig

config cpu_cfg;
  design work.cpu;
endconfig

config mem_cfg;
  design work.mem;
endconfig
