function [] = result_figure1(A, An1, An2, fs, ti, tf, savepath, name, Tk, GA1, GA2, s, Sk, x)
%RESULT_FIGURE1 - Creates a figure for the paper in noise reduction. Two
%different methods for estimating the noise level is compared. A suitable
%test signal is ... as the speech signal and ... as the noise signal.
%
% Syntax:  [] = result_figure1(A, An1, An2, fs, ti, tf, savepath, name, Tk, GA1, GA2, s, Sk)
%
% Inputs:
%    A - Short time average
%    An1 - Noise level estimation, Westerlund et al
%    An2 - Noise level estimation, proposed improvement
%    fs = Sampling frequency
%    ti - Beginning of desired section in speech data in seconds
%    tf - End of desired section in speech data in seconds
%    savepath - Directory where image should be stored
%    name - Output file name
%    Tk - Speech detection threshold vector
%    GA1 - Subband gain, Westerlund et al
%    GA2 - Subband gain, proposed improvement
%    s - Subband to be plotted
%    Sk - Ratio between max and min blocks
%    x - Time domain signal with both speech and noise
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
% Created - 2009-11-30
% Last modification - 2011-Aug-02

%------------- BEGIN CODE --------------

% Total length of data
L = size(A,2);

% Input arguments check
if( (ti*fs > L) || (tf*fs > L) )
    disp('ERROR in result_figure1: Time interval is out of bounds.')
    disp(['The length of the data is ',num2str(L/fs),' seconds'])
    return
elseif( ti>tf )
    disp('ERROR in result_figure1: Time interval start is larger then end time')
    return
end

% Determine start and stop positions
ni = floor(ti*fs);
nf = floor(tf*fs);
% Length of desired signal
Ld = nf-ni;
% Indices of desired interval
Id = [ni:nf];
% Time vector
time = Id./fs;

% Convert data to dB
A = 20*log10(A + eps);
An1 = 20*log10(An1 + eps);
An2 = 20*log10(An2 + eps);

% --- Plotting
NR_PLOTS = 4;
NR_PLOTS = NR_PLOTS-1; % (a) is removed
fontsize = 16+7;
fontleg = 12+7;
linewidth = 2;
% Create invisible figure
h = figure('visible','on');



leg1{1} = ['A_',num2str(s-1),'(n)'];
% leg1{2} = ['B_',num2str(s-1),'(n) : T_{\phi,',num2str(s-1),'} = \infty'];
leg1{2} = ['B_',num2str(s-1),'(n) : SBA'];
leg1{3} = ['B_',num2str(s-1),'(n) : Proposed'];
lines = {'k--','k:','k-.','k-'};

% ------ Plot (a) ------
% subplot(NR_PLOTS,1,1)
% set(gca, 'fontsize', fontsize);
% ha = gca;
% % Short time average
% plot(time,A(s,Id),'k','LineWidth', linewidth);
% hold on
leg2{1} = leg1{1};
% % Noise level estimate
% if(ndims(An2)<3)
%     plot(time,An2(s,Id),'k-.','LineWidth', linewidth)
% else
    for(i=1:size(An2,3))
%         plot(time,An2(s,Id,i),lines{i},'LineWidth', linewidth)
        % Legend entry
        leg2{i+1} = ['B_',num2str(s-1),'(n) : T_{\phi,',num2str(s-1),'} = ',num2str(Tk(i))];
    end
% end
% 
% % Axis
% xlabel('Time [s]')
% ylabel('Averages [dB]')
% title('(a)')
% % Insert legend
% h1 = legend(leg2);
% set(h1, 'fontsize', fontleg, 'location', 'SouthWest')


% ------ Plot (a) ------
subplot(NR_PLOTS,1,2-1)
set(gca, 'fontsize', fontsize);
hc = gca;
plot(time, 20*log10(GA1(s,Id)),'k--','LineWidth', linewidth)
hold on
i = 2;
plot(time, 20*log10(GA2(s,Id)),lines{i},'LineWidth', linewidth)
% Current y-axis maximum
% Gmax = 20*log10(max(max(GA1(s,Id)),max(GA1(s,Id))));
% axis([time(1), time(end), 0, 1.1*Gmax]);
v = axis;
v(3) = v(3)+4;
v(4) = v(4)+1;
axis(v);
axis('auto x');

% Axis
xlabel('Time [s]')
ylabel(['g_{1,',num2str(s-1),'}(n) [dB]'])
title('(a)')
% Insert legend
% leg2{1} = ['T_{\phi,',num2str(s-1),'} = \infty'];
% leg2{2} = ['T_{\phi,',num2str(s-1),'} = ',num2str(Tk(i))];
leg2{1} = ['SBA'];
leg2{2} = ['Proposed'];
h3 = legend(leg2{1},leg2{2});
set(h3, 'fontsize', fontleg, 'location', 'NorthWest')


% ------ Plot (b) ------
subplot(NR_PLOTS,1,3-1)
set(gca, 'fontsize', fontsize);
hb = gca;
% Short time average
plot(time,A(s,Id),'k','LineWidth', linewidth);
hold on
% Noise level estimate
plot(time,An1(s,Id),'k--','LineWidth', linewidth)
i = 2;
plot(time,An2(s,Id,i),lines{i},'LineWidth', linewidth)

% Axis
xlabel('Time [s]')
ylabel('Averages [dB]')
title('(b)')
% Insert legend
% h2 = legend(leg1{1},leg1{2},leg2{i+1});
h2 = legend(leg1{1},leg1{2},leg1{3});
set(h2, 'fontsize', fontleg, 'location', 'NorthWest')



% ------ Plot (c) ------
% Fullband signal sampling frequency
fs_FB = fs*32; % No downsampling
% Determine start and stop positions
ni_FB = floor(ti*fs_FB);
nf_FB = floor(tf*fs_FB);
% Indices of desired interval
Id_FB = [ni_FB:nf_FB];
% Time vector
time_FB = Id_FB./fs_FB;

subplot(NR_PLOTS,1,4-1)
set(gca, 'fontsize', fontsize);
% % Spectogram
% NFFT = 1024;
% [specRes, specF, specT] = specgram(x(Id_FB), NFFT, fs_FB);
% imagesc(specT, specF, 20*log10(abs(specRes)));
% axis xy
% tmp = colormap(gray); % Gray scale
% colormap(flipud(tmp)); % Inverse gray scale
% Time domain plot
plot(time_FB,x(Id_FB),'k')
% plot(time_FB,x(Id_FB),'k','LineWidth', linewidth)
xlabel('Time [s]')
ylabel('x(n)')
title('(c)')
% Correct axis so that y-scale goes from -1 to 1
tmp = axis;
tmp(3) = -1; tmp(4) = 1;
axis(tmp);


% ------ Plot (d) ------
if(NR_PLOTS==4)
    subplot(NR_PLOTS,1,4)
    set(gca, 'fontsize', fontsize);
    hd = gca;
    plot(time, Sk(s,Id),'k','LineWidth', linewidth)

    % Axis
    xlabel('Time [s]')
    ylabel(['S_{',num2str(s-1),'}(n)'])
    title('(d)')
end


% ------ Figure 2 ------
% Create invisible figure
h2 = figure('visible','on');

N = 4; % N+1 = # of subplots
for(i=0:N)
    subplot(1+N,1,1+i)
    set(gca, 'fontsize', fontsize);
    hd = gca;
    x = 2*i;
    plot(time, Sk(s + x,Id),'k','LineWidth', linewidth)

    % Axis
    xlabel('Time [s]')
    ylabel(['S_{',num2str(s-1 + x),'}(n)'])
    %title('(d)')
    
    % THIS IS REAALLY UGLY, PLEASE REMOVE
    if(strcmp(name,'result1a')==1)
        axis([20, 40, 0, 220])
    else
        if(strcmp(name,'result1b')==1)
            axis([15, 25, 0, 12])
        end
    end
end


% --- Screensize
% Fullscreen
fullscreen = get(0,'ScreenSize');
if(NR_PLOTS==4)
    X1 = 0.3;
else
    X1 = 0.2; % This is used -0.2
end
X2 = 0.3;
set(h,'Position',[0, -X1*fullscreen(4), 0.6*fullscreen(3), (1+X1)*fullscreen(4)]);
set(h2,'Position',[0, -X2*fullscreen(4), 0.6*fullscreen(3), (1+X2)*fullscreen(4)]);
% Use actual look when saving
set(h, 'PaperPositionMode', 'auto')
set(h2, 'PaperPositionMode', 'auto')

% Change margin sizes
% set(ha,'LooseInset',get(ha,'TightInset'))
% set(hb,'LooseInset',get(hb,'TightInset'))
% set(hc,'LooseInset',get(hc,'TightInset'))



% --- Results
currentpath = pwd;
% Save picture
cd(savepath);
% filemenufcn(h,'FileSaveAs')
saveas(h,name,'eps')
saveas(h2,[name,'_Sk'],'eps')
cd(currentpath);

% close(h);

%------------- END OF CODE --------------