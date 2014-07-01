function [F] = gains_vad(G, GL, GT, t)
%GAINS_VAD - Adjust subband gains so that if the gains have been below a
%threshold during a certain time it will immidiately rise to full gain if
%the threshold is reached.
%
% Syntax:  [F] = gains_vad(G, GL, GT, t)
%
% Inputs:
%    G - Subband gain
%    GL - Maximum gain in dB
%    GT - Gain threshold in dB
%    t - Required silent time in samples
%
% Outputs:
%    F - Output subband gain
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
% Created - 2009-11-24
% Last modification - 2009-11-24

%------------- BEGIN CODE --------------

% Number of subbands
Nsub = size(G,1);
% Data length for each subband
Lsub = size(G,2);
%
gamma = 0.99;

% Convert to dB
GL = 10^(GL/20);
GT = 10^(GT/20);

% Memory allocation
F = zeros(Nsub, Lsub);

% 
for(n=1+t:Lsub)
    for(k=1:Nsub)
        % Sort out (t-1) previous values
        vec = G(k,n-t:n-1);
        % If the gain has been low for a while and suddenly rises, go to
        % maximal gain
        if((max(vec)<GT) && (G(k,n)>=GT) )
            F(k,n) = GL;
            %disp(['[k,n] = ',num2str(k),', ',num2str(n)])
        else
            % F(k,n) = G(k,n);
            F(k,n) = gamma*F(k,n-1) + (1-gamma)*G(k,n);
        end
    end
end

%------------- END OF CODE --------------