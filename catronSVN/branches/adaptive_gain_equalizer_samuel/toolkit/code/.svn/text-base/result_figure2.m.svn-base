function [] = result_figure2(X, G1, G2, fs, savepath, TMdB, name)
%RESULT_FIGURE1 - Creates a figure for the paper in noise reduction
%
% Syntax:  [] = result_figure2(X, G1, G2, fs, savepath, TMdB)
%
% Inputs:
%    X - Input signal
%    G1 - Total subband gain, Method 5
%    G2 - Total subband gain, Method 7
%    fs = Sampling frequency
%    savepath - Directory where image should be stored
%    TMdB - Speech detection threshold [dB]
%    name - Output file name
%
% Outputs:
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
% Created - 2009-12-01
% Last modification - 2009-12-21

%------------- BEGIN CODE --------------

% Subband to be plotted
s = 2; %4
% Total length of data
L = size(X,2);
% Desired plot interval
Di = floor(0.35*L); % Initial
Df = floor(0.70*L); % Final
Id = [Di:Df];
% Length of desired interval
Ld = length(Id);
% Time vector
time = Id./fs;

% Create output
Y1 = X.*G1;
Y2 = X.*G2;

% Convert data to dB
%X = 20*log10(X + eps);
G1 = 20*log10(G1 + eps);
G2 = 20*log10(G2 + eps);

% Maxmimum amplitude
Amax = 1.2*max(real(X(s,Id)));

% --- Plotting
linewidth = 2;
% Create invisible figure
h = figure('visible','off');
% h = figure('visible','on');

subplot(4,1,1)
ax1 = gca;
% Short time average
plot(time,G1(s,Id),'k-.','LineWidth', linewidth)
hold on
plot(time,G2(s,Id),'k-','LineWidth', linewidth)

% subplot(3,1,2)
% plot(time, G1(s,Id)-G2(s,Id),'k','LineWidth', linewidth)

% subplot(4,1,2)
% ax2 = gca;
% % Input signal
% plot(time,real(X(s,Id)),'k','LineWidth', linewidth)

% Westerlund method
subplot(3,1,2)
ax3 = gca;
plot(time,real(Y2(s,Id)),'k','LineWidth', linewidth)

% Our method
subplot(3,1,3)
ax4 = gca;
plot(time,real(Y1(s,Id)),'k','LineWidth', linewidth)


% --- Make it pretty
fontsize = 16 + 4;
fontleg = 12-1.5 +4; % Fontsize in legends
set(ax1, 'fontsize', fontsize);
% set(ax2, 'fontsize', fontsize);
set(ax3, 'fontsize', fontsize);
set(ax4, 'fontsize', fontsize);

% Axis
xlabel(ax1,'Time [s]')
% xlabel(ax2,'Time [s]')
xlabel(ax3,'Time [s]')
xlabel(ax4,'Time [s]')

title(ax1,'(a)')
% title(ax2,'(b)')
title(ax3,'(b)')
title(ax4,'(c)')

ylabel(ax1,['G_',num2str(s-1),'(n) [dB]'])
% ylabel(ax2,['Re\{x_',num2str(s-1),'(n)\}'])
ylabel(ax3,['Re\{y_',num2str(s-1),'(n)\}'])
ylabel(ax4,['Re\{y_',num2str(s-1),'(n)\}'])


axis(ax1,[time(1), time(end), -30, 1]);
% axis(ax2,[time(1), time(end), -Amax, Amax]);

% set(ax2, 'YTickLabel', '')
set(ax3, 'YTickLabel', '')
set(ax4, 'YTickLabel', '')


%str = sprintf('$$\\varphi$$\\');
% str = sprintf('$$\\T_{varphi} = -%g dB$$\\%',10);
temp = 10^(-TMdB/20);
temp = floor(temp*100)/100;
% Create legend, put extra horisontal space so that surrounding box will
% enclose all text when converting to eps
hle = legend(ax1,['$T_{\varphi} = \textrm{Proposed}$','\hspace{8 mm}'], '$T_{\varphi} = \textrm{SBA}$');
% hle = legend(ax1,['$T_{\varphi} = ',num2str(temp),'$'], '$T_{\varphi} = 0$');
% hle = legend(ax1,str);
set(hle,'fontsize',fontleg)
set(hle,'Interpreter','latex');

% -- This below will not work
% Resize legend so that text fits
% pos = get(hle,'Position'); % 'OuterPosition'
% pos(3) = 0.004*pos(3); % Change width
% pos(4) = 0.001*pos(4); % Change heigth
% pos
% pos = [0.8, 0.8, 0.1, 0.1]
% set(hle,'Position',pos);

% Change back from italic
%set(hle, 'FontAngle','Normal')


% --- Screensize
% Fullscreen
fullscreen = get(0,'ScreenSize');
X1 = -0.1; % 0.3 if 4 plots
set(h,'Position',[0, -X1*fullscreen(4), 0.6*fullscreen(3), (1+X1)*fullscreen(4)]);
% Use actual look when saving
set(h, 'PaperPositionMode', 'auto')

% --- Results
currentpath = pwd;
% Save picture
cd(savepath);
saveas(h,name,'eps')
cd(currentpath);

close(h);

%------------- END OF CODE --------------