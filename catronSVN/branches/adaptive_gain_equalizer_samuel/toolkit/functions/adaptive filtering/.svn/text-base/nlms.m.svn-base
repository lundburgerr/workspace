function [yhat, hhat, e] = nlms(x, y, N, beta, hhat)
%NLMS - Normalized Mean Square algorithm
%
%
% Syntax:  [yhat, hhat, e] = nlms(x, y, N, beta, hhat)
%
% Inputs:
%    x - Loudspeaker signal
%    y - Microphone signal
%    N - Filter length
%    beta - Stepsize parameter
%    hhat - Initial filter state [optional]
%
% Outputs:
%    yhat - Estimated microphone signal
%    hhat - Estimated filter
%    e - Echo cancelled microphone signal
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
% E-mail - markus.borgh@limesaudio.com
% Website - www.limesaudio.com
% Copyright - Limes Audio AB
% Created - 2010-Aug-30
% Last modification - 2011-Apr-01

%------------- BEGIN CODE --------------

% ---< Settings >---

% ---<  >---
% Make sure input is column vector
x = x(:);
y = y(:);
% Is the initial filter state provided?
if(nargin > 4)
    hhatGiven = 1;
    hhat = hhat(:).';
    if(length(hhat)~=N)
        error('NLMS: Initial filter state have incorrect length');
    end
else
    hhatGiven = 0;
end

% ---<  >---
% Size of input
S = size(x);
L = max(S);

% ---< Memory allocation >---
yhat = zeros(S);
e = zeros(S);
if(~hhatGiven)
    hhat = zeros(1,N);
end

% ---<  >---
% Step-size in percent display (given in %-units)
perc_step = 10;
perc_counter = perc_step;
str = '';

for k = 2+N : L
    % Delay line
    xtemp = x(k:-1:k-N+1);
    
    % Estimate mic signal
    yhat(k) = hhat * xtemp;
    
    % Calculate error
    e(k) = y(k) - yhat(k);
    
    % Denominator for normalization
    denom = xtemp.'*conj(xtemp) + eps;
    
    % Update filter
    hhat = hhat + beta/denom*e(k)*conj(xtemp).';
    
    % Display how much has been completed
    t = floor(k/L*100);
    if(perc_counter == t)
        % Remove old value
        for(c=1:length(str))
            fprintf('\b');
        end
        % Display new time setting
        str = [num2str(t),'% completed'];
        fprintf('%s', str);
        perc_counter = perc_counter + perc_step;
    end
end

fprintf('\n');
% Close waitbar window
% close(h)

% Transform to column vector
hhat = hhat.';
% ---<  >---

%------------- END OF CODE --------------