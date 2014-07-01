function [b,F,A] = a_weighting(N, fs)
%A_WEIGHTING - Creates an A-weighting filter
%
% Syntax:  [b,F,A] = a_weighting(N, fs)
%
% Inputs:
%    N - Number of filter coefficients
%    fs - Sampling frequency
%
% Outputs:
%    b - FIR filter coefficients
%    F - Frequency vector for correct A-weighting (normalized to 1)
%    A - Amplitude vector for correct A-weighting (linear scale)
%
% Example: 
%    % Sampling frequency
%    fs = 44100;
%    % Number of coefficients
%    Ncoefs = 256;
%    % Create A-weigthing filter
%    [b,f_true,A_true] = a_weighting(Ncoefs,fs);
%
% Other m-files required: none
% Subfunctions: none
% MAT-files required: none

% Author - Markus Borgh
% E-mail - markus.borgh@limesaudio.com
% Website - www.limesaudio.com
% Copyright - Limes Audio AB
% Created - 2010-10-26
% Last modification - 2010-10-26

%------------- BEGIN CODE --------------

half_fs = fs/2;

% Filter order is one less than the number of coefficients
N = N-1;

% Frequency and amplitude vector is taken from the standard
F = [
    10, 12.5, 16, 20, 25,...
    31.5, 40, 50, 63, 80,...
    100, 125, 160, 200, 250,...
    315, 400, 500, 630, 800,...
    1000, 1250, 1600, 2000, 2500,...
    3150, 4000, 5000, 6300, 8000,...
    10000, 12500, 16000, 20000, half_fs];
% Normalize to 1
F = F/half_fs;

A = [
    -70.4, -63.4, -56.7, -50.5, -44.7,...
    -39.4, -34.6, -30.2, -26.2, -22.5,...
    -19.1, -16.1, -13.4, -10.9, -8.6,...
    -6.6, -4.8, -3.2, -1.9, -0.8,...
    0, 0.6, 1.0, 1.2, 1.3,...
    1.2, 1.0, 0.5, -0.1, -1.1,...
    -2.5, -4.3, -6.6, -9.3, -12];

% Convert to linear
A = 10.^(A./20);

% Since firls requires pairs of F and A, we need to duplicate some entries
L = length(F);
F2 = zeros(1,2*L-2);
A2 = zeros(1,2*L-2);

% F2(1) = F(1)
% F2(2) = F(2)
% F2(3) = F(2)
% F2(4) = F(3)
% F2(5) = F(3)
% F2(6) = F(4)
% F2(7) = F(4)
% F2(8) = F(5)
for(k=0:L-2)
    F2(2*k+1) = F(k+1);
    F2(2*k+2) = F(k+2);
    %     disp([num2str(2*k+1),' ',num2str(k+1)])
    %     disp([num2str(2*k+2),' ',num2str(k+2)])
    A2(2*k+1) = A(k+1);
    A2(2*k+2) = A(k+2);
end

% Store the new F and A with duplicate entries
F = F2;
A = A2;

% figure
% plot(F2,A2)

% Weight vector (this should be increased in low frequencies)
%W = ones(1,length(F)/2);
W = linspace(2,1,length(F)/2);

% Calculate the coefficients using the FIRLS function.
b  = firls(N, F, A, W, 'differentiator');
Hd = dfilt.dffir(b);

% Filter coefficients
b = Hd.Numerator;

%------------- END OF CODE --------------
