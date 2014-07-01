%
%

% Author - Markus Borgh
% E-mail - markus.borgh@limestechnology.com
% Website - www.limestechnology.com
% Copyright - Limes Technology AB
% Created - 2009-11-11
% Last modification - 2009-11-11

clear all, close all

DIR_DATA = 'Result\';
DIR_HOME = '..\';

filename = 'borgh_t0pink_sub24dB_sub12dBfull12dB.wav';

[x,fs] = wavread([DIR_DATA,filename]);

Ltot = length(x);
L = floor(Ltot/3);

% Divide signal into two. The input data is assumed to consist of the data
% at the beginning, only subband gain in second part and subband + fullband
% gain in the third part
x0 = x(1:L);
x1 = x(L+1:2*L);
x2 = x(2*L+1:end);


% Find delay between signals
[v,ind] = max(xcorr(x0,x1));
d1 = ind-L;
[v,ind] = max(xcorr(x0,x2));
d2 = ind-L;

% Adjust signals so they coincide



% Smoothing of signals
gamma = 0.9994;
x1_ave = smoothing(x1, gamma, gamma);
x2_ave = smoothing(x2, gamma, gamma);


% ----- Speech detection -----
% Threshold, this is correlated to gamma
T = 0.008;
ind = find(x1_ave>T);
speech = zeros(L,1);
speech(ind) = 1;

% Correlation




% ----- Plotting -----

plot(x1)
hold on
plot(x1_ave,'r')
plot(20*log10(speech),'g')


figure
plot(x2)
hold on
plot(x2_ave,'r')
plot(20*log10(speech),'g')


% n = [1:2*L-1]-L;
% figure
% plot(n,xcorr(x0,x1))
% hold on
% plot(n,xcorr(x0,x2),'r')
% legend('x_1','x_2')

figure
plot(20*log10(x1_ave))
hold on
plot(20*log10(x2_ave),'r')
plot(20*log10(x2_ave./x1_ave),'k')
%plot(1:L, ones(L,1)*0,'--m')
plot(20*log10(speech),'g')
legend('x_1','x_2','x_2 / x_1')

% figure,plot(x0), hold on, plot(x1,'k'), plot(x2,'m')
% legend('x_0','x_1','x_2')

% Figure positions
% cd(DIR_FUNC)
X_nr = 3;
Y_nr = 2;
[x_pos, y_pos, fg_width, fg_heigth] = figure_position(X_nr, Y_nr);
% cd(DIR_HOME) % Return to this directory
% Set figure positions
set(figure(1),'Position',[x_pos(1) y_pos(1) fg_width fg_heigth])
set(figure(2),'Position',[x_pos(2) y_pos(1) fg_width fg_heigth])
set(figure(3),'Position',[x_pos(1) y_pos(2) fg_width fg_heigth])
% set(figure(4),'Position',[x_pos(2) y_pos(2) fg_width fg_heigth])
% set(figure(5),'Position',[x_pos(3) y_pos(2) fg_width fg_heigth])

