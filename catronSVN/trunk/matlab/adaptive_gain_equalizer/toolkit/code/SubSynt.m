function [out, tdl] = SubSynt(in, N, L, R, p)
%SubSynt - Subband analysis
%
% Syntax:  [out, tdl] = SubSynt(in, N, L, R, p)
%
% Inputs:
%    in - Input data divided into subbands
%    N - Number of prototype filter coefficients
%    L - Length of non-decimated data - N
%    R - Decimation ratio
%    p - Filter bank
%
% Outputs:
%    out - N reconstructed output samples
%    tdl - updated TDL of state values
%
% Example: 
%    Line 1 of example
%    Line 2 of example
%    Line 3 of example
%
% Other m-files required: GDFTSynCmplx.m
% Subfunctions: none
% MAT-files required: none

% Author - Markus Borgh
% E-mail - markus.borgh@limestechnology.com
% Website - www.limestechnology.com
% Copyright - Limes Technology AB
% Created - 2009-10-26
% Last modification - 2009-10-26

%------------- BEGIN CODE --------------

% Memory allocation
out = zeros(1, L+R);
tdl = zeros(1, N);

for i=1:R:L
    [out(i:i+R-1) tdl] = GDFTSynCmplx_mod(in(:,((i-1)/R)+1), R, p, tdl);
end

%------------- END OF CODE --------------