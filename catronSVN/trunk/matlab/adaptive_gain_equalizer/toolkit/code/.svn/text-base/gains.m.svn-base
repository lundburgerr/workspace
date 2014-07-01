function [G] = gains(S, N, pow, L)
%gains - Calculates the gain using Westerlund et al. method from the ratio
%between short and long time average.
%
% Syntax:  [G] = gains(S, N, pow, L)
%
% Inputs:
%    S - Short time average (signal)
%    N - Long time average (noise)
%    pow - Power applied to gain for each subband
%    L - Maximum allowed gain
%
% Outputs:
%    G - Resulting gain
%
% Example: 
%    Line 1 of example
%    Line 2 of example
%    Line 3 of example
%
% Other m-files required: none
% Subfunctions: none
% MAT-files required: none

% Author - Markus Borgh
% E-mail - markus.borgh@limestechnology.com
% Website - www.limestechnology.com
% Copyright - Limes Technology AB
% Created - 2009-11-04
% Last modification - 2009-11-17

%------------- BEGIN CODE --------------
% Avoid division by zero
N = N + eps;

% Calculate gain
% G = (S./N).^pow(1);
G = (S./N);
% Apply power to each subband
Nsub = size(G,1);
for(n=1:Nsub)
    G(n,:) = G(n,:).^pow(n);
end

% No more gain than threshold value
ind = find(G>L);
G(ind) = L;

% Decreasing gain not allowed
ind = find(G<1);
G(ind) = 1;

%------------- END OF CODE --------------