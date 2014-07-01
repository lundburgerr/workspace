close all;

fs = 100000;
%func = @(a, b, x) 4./(a*x+b).^2;
%% Fit to exp decay
fid = fopen('EMeasurements/Ewithout_full.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
%[myfit, h] = velgeneral(laser1, laser2, fs, func, startpoint);
h = velfind(laser1, laser2, fs);
set(gca,'fontsize', 16, 'fontweight', 'demi');
saveas(h, 'report/img/resultExpNormal.png');
title('0skrew');

fid = fopen('EMeasurements/E10skrewCenter_full.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
% [myfit, h] = velgeneral(laser1, laser2, fs, func, startpoint);
h = velfind(laser1, laser2, fs);
set(gca,'fontsize', 16, 'fontweight', 'demi');
saveas(h, 'report/img/resultExpHeavy.png');
title('10skrew');
% myfit

fid = fopen('CMeasurements/Cardboard1+2_full.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
% [myfit, h] = velgeneral(laser1, laser2, fs, func, startpoint);
h = velfind(laser1, laser2, fs);
set(gca,'fontsize', 16, 'fontweight', 'demi');
saveas(h, 'report/img/resultExpDraggy.png');
title('draggy');
% myfit

%% Fit to tan decay
func = @(b, c, x) tan(-b*(x+c)).^2;
startpoint = [0.011,100*pi/2];

fid = fopen('EMeasurements/Ewithout_full.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
[myfit, h] = velgeneral(laser1, laser2, fs, func, startpoint);
set(gca,'fontsize', 16, 'fontweight', 'demi');
%h = velfind(laser1, laser2, fs);
saveas(h, 'report/img/resultTanNormal.png');
title('0skrew');

fid = fopen('EMeasurements/E10skrewCenter_full.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
[myfit, h] = velgeneral(laser1, laser2, fs, func, startpoint);
set(gca,'fontsize', 16, 'fontweight', 'demi');
%h = velfind(laser1, laser2, fs);
saveas(h, 'report/img/resultTanHeavy.png');
title('10skrew');
% myfit

fid = fopen('CMeasurements/Cardboard1+2_full.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
[myfit, h] = velgeneral(laser1, laser2, fs, func, startpoint);
set(gca,'fontsize', 16, 'fontweight', 'demi');
%h = velfind(laser1, laser2, fs);
saveas(h, 'report/img/resultTanDraggy.png');
title('draggy');
% myfit