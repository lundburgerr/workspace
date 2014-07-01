% RESULT_SDI

close all
clear all

DIR_HOME = [pwd,'\'];
DIR_RES = [DIR_HOME,'result\'];

% Pasted from excel sheet "sdi.xlsx" - ver3
X = [1	0.0	5.7	5.0	-6.4	-7.3	5.8	5.0	-12.2	-12.3	2.126	2.579	2.571
3	0.0	6.1	5.1	-5.7	-7.2	6.1	5.1	-11.9	-12.3	2.126	2.563	2.568
6	0.0	6.2	5.2	-5.4	-7.0	6.3	5.2	-11.6	-12.2	2.126	2.535	2.572
9	0.0	6.3	5.2	-5.1	-7.0	6.3	5.2	-11.4	-12.2	2.126	2.507	2.572
1	6.0	11.2	10.2	-9.3	-10.8	5.2	4.3	-14.5	-15.0	2.682	2.862	2.852
3	6.0	11.9	10.3	-7.9	-10.6	5.9	4.4	-13.8	-15.0	2.682	2.858	2.849
6	6.0	12.3	10.4	-7.0	-10.5	6.3	4.4	-13.3	-14.9	2.682	2.837	2.851
9	6.0	12.5	10.4	-6.6	-10.4	6.5	4.5	-13.1	-14.9	2.682	2.817	2.852
1	12.0	15.4	14.4	-14.0	-16.4	3.4	2.4	-17.4	-18.8	2.96	3.068	3.068
3	12.0	16.6	14.5	-11.4	-16.2	4.6	2.5	-16.0	-18.7	2.96	3.09	3.069
6	12.0	17.3	14.5	-9.4	-16.1	5.4	2.5	-14.7	-18.7	2.96	3.087	3.07
9	12.0	17.7	14.5	-8.5	-16.0	5.7	2.6	-14.2	-18.6	2.96	3.069	3.07
1	18.0	19.2	18.6	-21.6	-24.9	1.2	0.6	-22.8	-25.6	3.183	3.257	3.295
3	18.0	20.4	18.6	-16.9	-24.9	2.4	0.6	-19.3	-25.5	3.183	3.294	3.298
6	18.0	21.8	18.6	-12.7	-24.9	3.8	0.6	-16.5	-25.5	3.183	3.331	3.299
9	18.0	22.4	18.6	-10.6	-24.9	4.5	0.6	-15.1	-25.5	3.183	3.332	3.3
1	24.0	24.3	24.2	-30.1	-31.7	0.3	0.2	-30.5	-31.9	3.423	3.493	3.569
3	24.0	24.7	24.2	-26.4	-31.7	0.7	0.2	-27.1	-31.9	3.423	3.515	3.572
6	24.0	25.8	24.2	-18.0	-31.7	1.8	0.2	-19.8	-31.9	3.423	3.536	3.573
9	24.0	26.8	24.2	-13.8	-31.7	2.8	0.2	-16.6	-31.9	3.423	3.557	3.573
1	-0.1	7.1	6.3	-9.1	-10.2	7.2	6.4	-16.2	-16.6	2.694	2.853	2.814
3	-0.1	7.4	6.4	-7.8	-9.9	7.6	6.5	-15.4	-16.4	2.694	2.859	2.819
6	-0.1	7.5	6.4	-7.1	-9.7	7.7	6.5	-14.7	-16.3	2.694	2.844	2.818
9	-0.1	7.5	6.5	-6.7	-9.6	7.7	6.6	-14.3	-16.2	2.694	2.832	2.812
1	5.9	12.9	11.8	-12.3	-13.6	7.0	5.9	-19.4	-19.5	2.957	3.06	3.079
3	5.9	13.7	11.9	-10.6	-13.4	7.8	6.1	-18.4	-19.4	2.957	3.076	3.087
6	5.9	13.9	12.0	-9.1	-13.2	8.1	6.2	-17.2	-19.4	2.957	3.075	3.086
9	5.9	14.1	12.1	-8.3	-13.1	8.2	6.2	-16.5	-19.3	2.957	3.068	3.082
1	11.9	17.1	15.8	-15.6	-17.4	5.2	3.9	-20.9	-21.3	3.204	3.283	3.385
3	11.9	18.6	15.9	-13.6	-17.2	6.7	4.0	-20.3	-21.2	3.204	3.302	3.389
6	11.9	19.5	16.0	-11.6	-17.0	7.6	4.1	-19.2	-21.1	3.204	3.32	3.391
9	11.9	19.7	16.1	-10.1	-16.8	7.9	4.2	-18.0	-21.0	3.204	3.313	3.391
1	17.9	20.0	19.0	-21.8	-25.5	2.2	1.1	-24.0	-26.6	3.419	3.553	3.682
3	17.9	21.9	19.1	-17.5	-24.5	4.0	1.2	-21.5	-25.7	3.419	3.571	3.671
6	17.9	23.6	19.2	-14.5	-23.9	5.7	1.3	-20.1	-25.2	3.419	3.589	3.672
9	17.9	24.5	19.2	-12.6	-23.6	6.7	1.4	-19.2	-24.9	3.419	3.597	3.673
1	23.9	24.2	24.0	-31.5	-33.0	0.3	0.2	-31.9	-33.2	3.643	3.922	3.995
3	23.9	24.9	24.1	-25.6	-32.9	1.0	0.2	-26.7	-33.1	3.643	3.942	3.996
6	23.9	26.5	24.1	-18.4	-32.8	2.7	0.2	-21.1	-32.9	3.643	3.957	3.997
9	23.9	27.9	24.1	-15.3	-32.6	4.0	0.2	-19.4	-32.8	3.643	3.96	3.998
1	-0.1	6.8	6.0	-8.8	-9.9	6.8	6.0	-15.6	-15.9	2.793	2.917	2.879
3	-0.1	7.3	6.2	-7.6	-9.7	7.4	6.2	-14.9	-16.0	2.793	2.916	2.875
6	-0.1	7.4	6.2	-6.8	-9.6	7.5	6.3	-14.3	-15.9	2.793	2.899	2.873
9	-0.1	7.5	6.3	-6.5	-9.5	7.6	6.3	-14.1	-15.9	2.793	2.884	2.873
1	5.9	12.5	11.4	-12.1	-13.5	6.6	5.5	-18.6	-19.0	3.043	3.138	3.121
3	5.9	13.4	11.6	-10.3	-13.4	7.4	5.6	-17.7	-19.0	3.043	3.159	3.122
6	5.9	13.7	11.6	-8.8	-13.3	7.8	5.7	-16.6	-19.0	3.043	3.158	3.124
9	5.9	13.9	11.7	-8.1	-13.2	7.9	5.8	-16.0	-18.9	3.043	3.143	3.126
1	11.9	16.8	15.5	-15.5	-17.7	4.8	3.5	-20.4	-21.2	3.273	3.35	3.379
3	11.9	18.3	15.7	-13.2	-17.2	6.4	3.8	-19.6	-21.0	3.273	3.384	3.38
6	11.9	19.1	15.8	-11.3	-17.1	7.2	3.9	-18.5	-20.9	3.273	3.408	3.382
9	11.9	19.4	15.9	-9.9	-16.9	7.5	3.9	-17.4	-20.9	3.273	3.4	3.382
1	17.9	19.9	18.9	-22.0	-26.3	2.0	0.9	-24.0	-27.3	3.535	3.602	3.676
3	17.9	21.8	19.1	-17.0	-24.9	3.8	1.2	-20.8	-26.0	3.535	3.626	3.677
6	17.9	23.3	19.1	-14.0	-24.4	5.3	1.2	-19.3	-25.7	3.535	3.656	3.679
9	17.9	24.1	19.2	-12.3	-24.3	6.1	1.2	-18.4	-25.5	3.535	3.668	3.679
1	23.9	24.2	24.1	-32.3	-33.5	0.3	0.2	-32.6	-33.7	3.779	3.914	3.988
3	23.9	24.9	24.1	-25.5	-32.9	1.0	0.2	-26.5	-33.1	3.779	3.931	3.989
6	23.9	26.6	24.1	-17.8	-32.7	2.7	0.2	-20.5	-32.9	3.779	3.946	3.99
9	23.9	27.9	24.1	-14.8	-32.6	4.0	0.2	-18.7	-32.8	3.779	3.952	3.99
1	-0.3	0.0	-0.1	-10.3	-18.1	0.3	0.2	-10.6	-18.2	1.849	1.814	1.864
3	-0.3	0.2	-0.1	-7.3	-18.1	0.5	0.2	-7.7	-18.2	1.849	1.879	1.864
6	-0.3	0.4	-0.1	-5.8	-18.1	0.7	0.2	-6.6	-18.2	1.849	1.912	1.864
9	-0.3	0.5	-0.1	-5.2	-18.1	0.8	0.2	-5.9	-18.2	1.849	1.95	1.864
1	5.7	6.3	5.9	-14.7	-24.6	0.6	0.2	-15.3	-24.7	2.129	2.178	2.137
3	5.7	6.8	5.9	-10.4	-24.6	1.1	0.2	-11.5	-24.7	2.129	2.203	2.137
6	5.7	7.2	5.9	-7.8	-24.6	1.5	0.2	-9.3	-24.7	2.129	2.238	2.137
9	5.7	7.5	5.9	-6.8	-24.6	1.8	0.2	-8.6	-24.7	2.129	2.25	2.137
1	11.7	12.1	11.8	-21.5	-32.6	0.4	0.1	-21.9	-32.6	2.483	2.512	2.479
3	11.7	12.7	11.8	-14.9	-32.6	1.0	0.1	-15.9	-32.6	2.483	2.542	2.479
6	11.7	13.3	11.8	-11.0	-32.6	1.6	0.1	-12.7	-32.6	2.483	2.597	2.479
9	11.7	13.7	11.8	-9.1	-32.6	2.0	0.1	-11.0	-32.6	2.483	2.622	2.479
1	17.7	17.9	17.7	-30.5	-38.1	0.2	0.0	-30.7	-38.2	2.842	2.841	2.829
3	17.7	18.4	17.7	-21.3	-38.1	0.7	0.0	-21.9	-38.2	2.842	2.867	2.829
6	17.7	18.9	17.7	-15.0	-38.1	1.2	0.0	-16.3	-38.2	2.842	2.903	2.829
9	17.7	19.4	17.7	-11.9	-38.1	1.7	0.0	-13.6	-38.2	2.842	2.919	2.829
1	23.7	23.8	23.7	-37.3	-43.3	0.1	0.0	-37.4	-43.3	3.131	3.13	3.117
3	23.7	24.1	23.7	-30.0	-43.3	0.4	0.0	-30.4	-43.3	3.131	3.136	3.117
6	23.7	24.5	23.7	-21.0	-43.3	0.8	0.0	-21.8	-43.3	3.131	3.186	3.117
9	23.7	25.0	23.7	-15.3	-43.3	1.3	0.0	-16.6	-43.3	3.131	3.191	3.117
];

colInfo = {'beta','iSNR','oSNRsba','oSNRprop','SDIsba','SDIprop'};

% Number of different beta and iSNR
Nbetas = 4;
NiSNR = 5;

% Row info
rowCar = [1:Nbetas*NiSNR];
rowComp = [Nbetas*NiSNR + 1 : 2*Nbetas*NiSNR];
rowVent = [2*Nbetas*NiSNR + 1 : 3*Nbetas*NiSNR];
rowCock = [3*Nbetas*NiSNR + 1 : 4*Nbetas*NiSNR];

% Column info
colBeta = 1;
coliSNR = 2;
coloSNRsba = 3;
coloSNRprop = 4;
colSDIsba = 5;
colSDIprop = 6;
colPESQnp = 11;
colPESQsba = 12;
colPESQprop = 13;

% Sort data
iSNR = X(:,coliSNR);
oSNRsba = X(:,coloSNRsba);
oSNRprop = X(:,coloSNRprop);
SDIsba = X(:,colSDIsba);
SDIprop = X(:,colSDIprop);
PESQnp = X(:,colPESQnp);
PESQsba = X(:,colPESQsba);
PESQprop = X(:,colPESQprop);

% SNR gain
gSNRsba = oSNRsba - iSNR;
gSNRprop = oSNRprop - iSNR;


% Rise of "beta" used in calculations
temp = X(1:Nbetas, colBeta);
betaText = {}; % init
for(m=1:Nbetas)
    betaText{m} = [num2str(temp(m)), ' dB/s'];
end


% Legend fix...
dummyX = 200:210;
dummyY = dummyX;
for(f=1:3)
    figure(f)
    for(s=1:4)
        subplot(2,2,s)
        plot(dummyX, dummyY, 'ks')
        hold on
        plot(dummyX, dummyY, 'kv')
        plot(dummyX, dummyY, 'ko')
        plot(dummyX, dummyY, 'kx')
        if(f==2 && s==4)
            % Don't block the information in this subplot
            legend('1 dB/s', '3 dB/s', '6 dB/s', '9 dB/s', 'location', 'NorthEast')
        else
            legend('1 dB/s', '3 dB/s', '6 dB/s', '9 dB/s', 'location', 'SouthWest')
        end
    end
end


indMat = [rowCar; rowComp; rowVent; rowCock];
% plotSet = {'sb-','sr-','vb-','vr-','ob-','or-','xb-','xr-'}; % Color
plotSet = {'sk-.','sk-','vk-.','vk-','ok-.','ok-','xk-.','xk-'}; % Black and white
for(k=1:4)
    % Init plot settings
    plotSetCount = 1;
    
    figure(1)
    subplot(2,2,k)
    hold on
    figure(2)
    subplot(2,2,k)
    hold on
    figure(3)
    subplot(2,2,k)
    hold on
    for(m=1:Nbetas)
        % Group by beta=1,3,6,9
        ind = indMat(k, m:Nbetas:end);
        % == SDI ==
        figure(1)
        plot(iSNR(ind), SDIsba(ind), plotSet{plotSetCount})
        plotSetCount = plotSetCount + 1;
        plot(iSNR(ind), SDIprop(ind), plotSet{plotSetCount})
        plotSetCount = plotSetCount + 1;
        legText1{m} = ['SBA, ',betaText{m}];
        legText2{m} = ['Prop, ',betaText{m}];
        % == SNR gain ==
        figure(2)
        plotSetCount = plotSetCount - 2; % Decrease two too plot the same line settings
        plot(iSNR(ind), gSNRsba(ind), plotSet{plotSetCount})
        plotSetCount = plotSetCount + 1;
        plot(iSNR(ind), gSNRprop(ind), plotSet{plotSetCount})
        plotSetCount = plotSetCount + 1;
        legText1{m} = ['SBA, ',betaText{m}];
        legText2{m} = ['Prop, ',betaText{m}];
        % == PESQ gain ==
        figure(3)
        plotSetCount = plotSetCount - 2; % Decrease two too plot the same line settings
        plot(iSNR(ind), PESQsba(ind), plotSet{plotSetCount})
        plotSetCount = plotSetCount + 1;
        plot(iSNR(ind), PESQprop(ind), plotSet{plotSetCount})
        plotSetCount = plotSetCount + 1;
        legText1{m} = ['SBA, ',betaText{m}];
        legText2{m} = ['Prop, ',betaText{m}];
    end
    figure(1)
    ylabel('\nu_{sd} [dB]')
    xlabel('iSNR [dB]')
    if(k==1)
        %legend(legText1{1}, legText2{1}, legText1{2}, legText2{2}, legText1{3}, legText2{3}, legText1{4}, legText2{4}, 'location', 'SouthWest')
    end
    axis([-1, 25, -45, 0])
    grid on
    figure(2)
    ylabel('gSNR [dB]')
    xlabel('iSNR [dB]')
    if(k==1)
        %legend(legText1{1}, legText2{1}, legText1{2}, legText2{2}, legText1{3}, legText2{3}, legText1{4}, legText2{4}, 'location', 'SouthWest')
    end
    axis([-1, 25, -1, 10])
    grid on
    figure(3)
    ylabel('PESQMOS')
    xlabel('iSNR [dB]')
    if(k==1)
        %legend(legText1{1}, legText2{1}, legText1{2}, legText2{2}, legText1{3}, legText2{3}, legText1{4}, legText2{4}, 'location', 'SouthWest')
    end
    axis([-1, 25, 1.7, 4.1])
    grid on
end
figure(1)
subplot(2,2,1), title('(a)')
subplot(2,2,2), title('(b)')
subplot(2,2,3), title('(c)')
subplot(2,2,4), title('(d)')
figure(2)
subplot(2,2,1), title('(a)')
subplot(2,2,2), title('(b)')
subplot(2,2,3), title('(c)')
subplot(2,2,4), title('(d)')
% subplot(2,2,1), title('gSNR, Car noise')
% subplot(2,2,2), title('gSNR, Computer fan noise')
% subplot(2,2,3), title('gSNR, Ventilation noise')
% subplot(2,2,4), title('gSNR, Cocktail noise')
figure(3)
subplot(2,2,1), title('PESQMOS, Car noise')
subplot(2,2,2), title('PESQMOS, Computer fan noise')
subplot(2,2,3), title('PESQMOS, Ventilation noise')
subplot(2,2,4), title('PESQMOS, Cocktail noise')


% Legend fix...
figure(4)
dummyX = 200:210;
dummyY = dummyX;
% plot(dummyX, dummyY, 'k-.')
% plot(dummyX, dummyY, 'k-')
plot(dummyX, dummyY, 'ks')
hold on
plot(dummyX, dummyY, 'kv')
plot(dummyX, dummyY, 'ko')
plot(dummyX, dummyY, 'kx')
axis([-1, 25, -1, 12])
% legend('SBA', 'Prop', '1 dB/s', '3 dB/s', '6 dB/s', '9 dB/s')
legend('1 dB/s', '3 dB/s', '6 dB/s', '9 dB/s')


% Figure positions
X_nr = 2;
Y_nr = 2;
[x_pos, y_pos, fg_width, fg_heigth] = figure_position(X_nr, Y_nr);

fignr = 1; % Init
for m=1:Y_nr
    for n=1:X_nr
        set(figure(fignr),'Position',[x_pos(n) y_pos(m) fg_width fg_heigth]);
        % Increase figure nr
        fignr = fignr + 1;
    end
end


% Save figures
name = [DIR_RES, 'sdi'];
saveas(figure(1), name, 'eps');
name = [DIR_RES, 'snrgain'];
saveas(figure(2), name, 'eps');
