`timescale 1ns / 1ps

module top(
    input TRI_IN,
    output TRI_OUT,
    input TRI_CTRL,
    inout TRI_INOUT
);

   IOBUF iobuf_i (
    .I(TRI_IN),
    .IO(TRI_INOUT),
    .O(TRI_OUT),
    .T(TRI_CTRL)
    );
endmodule 
