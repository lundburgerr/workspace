function [y] = fill_up_with_const(x, Ld, c)
%FILL_UP_WITH_CONST - Applies constant data padding to input signal vector
%
% Syntax:  [y] = fill_up_with_const(x, Ld, c)
%
% Inputs:
%    x - Vector input signal
%    Ld - Desired length of output in samples
%    c - Data constant
%
% Outputs:
%    y - Output signal
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
% Created - 2009-11-19
% Last modification - 2009-11-19

%------------- BEGIN CODE --------------

if(size(x,1)==1)
    % Column vector
    type = 1;
elseif(size(x,2)==1)
    % Row vector
    type = 2;
else
    disp('ERROR in fill_up_with_zeros: Input not a vector');
    return
end

% Create constant data
z = ones(1, Ld-length(x)).*c;

% Zero padding
if(type == 1)
    y = [x, z];
else
    y = [x; z.'];
end

%------------- END OF CODE --------------