function [out] = SubAnal(in, N, M, R, p)
%SubAnal - Decimation and subband splitting
%
% Syntax:  [out] = SubAnal(in, N, M, R, p)
%
% Inputs:
%    in - Input data vector to be splitted into subbands
%    N - Number of prototype filter coefficients
%    M - Number of subbands
%    R - Decimation ratio
%    p - Filter bank
%
% Outputs:
%    out - Matrix containing subband divided decimated data
%
% Example: 
%    Line 1 of example
%    Line 2 of example
%    Line 3 of example
%
% Other m-files required: GDFTAnaCmplx_mod.m
% Subfunctions: none
% MAT-files required: none

% Author - Markus Borgh
% E-mail - markus.borgh@limestechnology.com
% Website - www.limestechnology.com
% Copyright - Limes Technology AB
% Created - 2009-10-26
% Last modification - 2009-10-26

%------------- BEGIN CODE --------------

L = length(in);

% Memory allocation
out = zeros(M, ceil(L/R));

% Decimate and perform subband analysis
for i=N:R:L
    x_block = in(i:-1:i-N+1)';
    % Subband
    y1 = GDFTAnaCmplx_mod(x_block, M, p);
	out(:,((i-N)/R)+1)= y1.';
end

%------------- END OF CODE --------------