function [plotData,plotDataFullband,coefs,DataPlotNames,INTERVAL,NR_FULLBAND_DATA,START_SUBBAND,NR_SUBBANDS]= getdumpdata (fname)
tic
fid1 = fopen(fname);
fid2 = fopen('temp.dat','w+')
tline = fgetl(fid1);
tline = fgetl(fid1);
while ischar(tline)  
    fwrite(fid2,tline);
    fprintf(fid2,'\n');
    tline = fgetl(fid1);    
end
t0=toc
fclose(fid2);
fclose(fid1);
data=load('temp.dat');
% Read first header data
NR_HEADER_DATA=data(1);
NR_FULLBAND_DATA=data(2);
NR_SUB_DATA=data(3);
NAME_LENGTH=data(4);
SUB_BUFFERLENGTH=data(5)
INTERVAL=data(6);
NR_BOOL_DATA=data(7)
NR_BOOL_DATA_2=data(8)
END_INDEX=data(9)
NR_SUBBANDS=data(10)
START_SUBBAND=data(11)

data=data(NR_HEADER_DATA+1:end);
NumData=NR_FULLBAND_DATA+NR_SUB_DATA+NR_BOOL_DATA+NR_BOOL_DATA_2;

for i=1:NumData
    namedata=data((i-1)*NAME_LENGTH+1:(i)*NAME_LENGTH);
    name=namedata(find(namedata~=0));
    DataPlotNames{i}=native2unicode(name)';
end
data=data(NumData*NAME_LENGTH+1:end);


%booleans
datablock=data(1:SUB_BUFFERLENGTH*NR_SUBBANDS);
for i=1:NR_BOOL_DATA
    bool=datablock-(floor(datablock/2)*2);
    for j=1:NR_SUBBANDS
        k=START_SUBBAND+j;
        plotData{i,k}=bool(j:NR_SUBBANDS:(SUB_BUFFERLENGTH-1)*NR_SUBBANDS+j);
    end
    datablock=floor(datablock/2);
end
data=data(SUB_BUFFERLENGTH*NR_SUBBANDS+1:end);
datablock=data(1:SUB_BUFFERLENGTH*NR_SUBBANDS);
for i=1:NR_BOOL_DATA_2
    bool=datablock-(floor(datablock/2)*2);
    for j=1:NR_SUBBANDS
        k=START_SUBBAND+j;
        plotData{i+NR_BOOL_DATA,k}=bool(j:NR_SUBBANDS:(SUB_BUFFERLENGTH-1)*NR_SUBBANDS+j);
    end
    datablock=floor(datablock/2);
end
data=data(SUB_BUFFERLENGTH*NR_SUBBANDS+1:end);

tic
%subplots
for i=1:NR_SUB_DATA
    datablock=data(SUB_BUFFERLENGTH*NR_SUBBANDS*(i-1)+1:SUB_BUFFERLENGTH*NR_SUBBANDS*(i));

    for j=1:NR_SUBBANDS
        k=START_SUBBAND+j;
        plotData{i+NR_BOOL_DATA+NR_BOOL_DATA_2,k}=datablock(j:NR_SUBBANDS:(SUB_BUFFERLENGTH-1)*NR_SUBBANDS+j);
    end
end
data=data(NR_SUB_DATA*SUB_BUFFERLENGTH*NR_SUBBANDS+1:end);
t1=toc
tic
%fullband plots

for i=1:NR_FULLBAND_DATA
    datablock=data(SUB_BUFFERLENGTH*(i-1)+1:SUB_BUFFERLENGTH*(i));
    %plot(datablock)
    %pause
    plotDataFullband{i}=datablock;
    % even though we have fullband data it is convinient to save (the same data ) in all
    % subbands for easier plotting
    for j=1:NR_SUBBANDS
        k=START_SUBBAND+j;
        plotData{i+NR_BOOL_DATA+NR_BOOL_DATA_2+NR_SUB_DATA,k}=datablock;
    end
    i+NR_BOOL_DATA+NR_BOOL_DATA_2+NR_SUB_DATA
end
t2=toc
%reposition the data with the help of END_INDEX

[a,b]=size(plotData)
for i=1:a
    for j=START_SUBBAND+1:b
        temp=plotData{i,j}(END_INDEX+2:end);
        temp2=plotData{i,j}(1:END_INDEX+1);
        L1=length(temp);
        L2=length(temp2);
        plotData{i,j}(1:L1)=temp;
        plotData{i,j}(L1+1:L1+L2)=temp2;
    end
end

%plotData(:,:,)=plotData(:,:,END_INDEX+2:end),plotData(:,:,1:END_INDEX+1)];

coefs=0;



