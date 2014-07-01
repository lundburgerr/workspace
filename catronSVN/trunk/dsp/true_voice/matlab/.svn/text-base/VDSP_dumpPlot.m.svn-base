clear all, close all

% Should AEC filters be plotted

PLOT_FILTERS=0
fname='dumpdata.dat';


[plotData,plotDataFullband,coefs,DataPlotNames,INTERVAL,NR_FULLBAND_DATA,START_SUBBAND,NR_SUBBANDS]= getdumpdata(fname);
%warning off

 %colors for the plots, in order
col={'b','g','r','m','k','c--','y--','r--','m--','k--'};
% What subbands should be plotted
plotsubNR=[START_SUBBAND+1:START_SUBBAND+NR_SUBBANDS];

%special plots


for j=1:length(plotsubNR)
            sub=plotsubNR(j);
 %            plotData{1,sub}=plotData{1,sub}*2;

end


%which plots to compare? for numbering of variables run once
for i=1:length(DataPlotNames)
    temp=[num2str(i),'. ',DataPlotNames{i}];
    temp=[temp,'                                '];
    NumName(i,:)=temp(1:20);
end
NumName  
%% Plot Settings (which plots should be shown)

ComparePlots={% show in one plot, numbering as the name numbers.
{9,10,29,15,16,11}
{17,18,19,'db'}
{11,10,29}
{23,11}


};
Resize=[ %resizes the specific plot data by this factor
   1;1;1;1;1; %1.01;1.02;1.03;1.04;1.05;
   1;1;1;1;1; %1.06;1.07;1.08;1.09;1.10; %10
   1;1;1;1;1;
   1;1/32767;1/32767;1/32767;1;%20
   1;1;1;1;1;
   1;1;1;1;1; %30
   1;1;1;1;1;
   1;1;1;1;1;
   1;1;1;1;1;
   1;1;1;1;1;
   1;1;1;1;1;
   1;1;1;1;1;
   ];
   
%% Plotting   
NR_compPlots=length(ComparePlots);

% Check if some plots should be made in dB
dbPlots = zeros(NR_compPlots, 1);
for i=1:NR_compPlots
    % Check if 'db' entry exists
    if(strcmp('db', ComparePlots{i}{end}) || strcmp('dB', ComparePlots{i}{end}) )
        dbPlots(i) = 1;
        % Remove 'db' entry from ComparePlots
        ComparePlots{i} = {ComparePlots{i}{1:end-1}};
    end
end

%% Subband signal plotting
for i=1:NR_compPlots
    figure,hold on
    tit=[];leg=[];min_y=0;max_y=0;
    
    for k=1:length(ComparePlots{i})
        y=[];
        for j=1:length(plotsubNR)
            sub=plotsubNR(j);
            temp=plotData{ComparePlots{i}{k},sub};
            y=[y;temp];
        end
        x=length(plotsubNR)/length(y):length(plotsubNR)/length(y):length(plotsubNR);
        y=Resize(ComparePlots{i}{k})*y;
        % Regular plot or dB-plot
        if(dbPlots(i))
            y = 20*log10(y); % Just overwrite
        end
        plot(x,y,col{k})
        %grid on
        
        min_y=min(min_y,min(y(~isinf(y)))); % Omit -Inf values that occur when plotting in dB
        max_y=max(max_y,max(y(~isinf(y))));
        
        tit=[tit,DataPlotNames{ComparePlots{i}{k}},', '];
        leg=[leg,DataPlotNames(ComparePlots{i}{k})];
        %tit=strcat(tit,DataPlotNames{ComparePlots{i}(k)},', ');
        %leg=[leg,DataPlotNames(ComparePlots{i}(k))];
    end
    
    tit = tit(1:end-2); % Remove last ', '
    % Append dB to title in db plots
    if(dbPlots(i))
        tit = [tit, ' [dB]'];
    end
    legend(leg)
    title(tit)
    
    % Separate each subband by dashed lines
    L=length(y);
    if(isempty(min_y))
        min_y=0;
    end
    if(isempty(max_y))
        max_y=0;
    end
    for j=1:length(plotsubNR)+1
        plot([(j-1),(j-1)],[min_y,max_y],'m-.');
    end
    
    % Name figures
    h=gcf; set(h,'Name',tit);
end

%% plotting coefficients
if(PLOT_FILTERS)
    h1=figure;
    h2=figure;
    h3=figure;
    for k=1:2
        figure(h1)
        subplot(1,2,k)
        hold on
        for i=1:length(coefs)
            coefs2(:,i)=real(coefs{k,i});
            plot(coefs2(:,i))
            for j=1:length(coefs)
                missalign(i,j)=sum(abs(coefs{k,i}-coefs{k,j}).^2)/(sum(abs(coefs{k,j}).^2+eps));
            end
        end
        title('Filter Coefficients')
        [y,I]=min(sum(missalign,1));
        min_miss=missalign(:,I);
        figure(h2)
        subplot(1,2,k)
        plot(10*log10(min_miss))
        title(['Missalignment using frame ',num2str(I),' as reference'])
        figure(h3)
        subplot(1,2,k)
        mesh(coefs2(end:-1:1,:))
        colorbar();
        title('Filter Coefficients over time')
    end
end % PLOT_FILTERS

arrange_figures(gcf, 2); % Arrange figures in two columns