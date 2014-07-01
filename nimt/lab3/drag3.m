close all;

fs = 100000;
func = @(a, b, x) 4./(a*x+b).^2;
startpoint = [1,1];
%% Fit to exp decay
fid = fopen('CMeasurements/Cardboard1+2_full.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
[myfit, h] = velgeneral(laser1, laser2, fs, func, startpoint);
set(gca,'fontsize', 16, 'fontweight', 'demi');
saveas(h, 'report/img/resultSquare1+2.png');
%h = velfind(laser1, laser2, fs);
title('1+2');

fid = fopen('CMeasurements/Cardboard1_full.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
[myfit, h] = velgeneral(laser1, laser2, fs, func, startpoint);
set(gca,'fontsize', 16, 'fontweight', 'demi');
saveas(h, 'report/img/resultSquare1.png');
%h = velfind(laser1, laser2, fs);
title('1');

fid = fopen('CMeasurements/Cardboard2_full.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
[myfit, h] = velgeneral(laser1, laser2, fs, func, startpoint);
set(gca,'fontsize', 16, 'fontweight', 'demi');
saveas(h, 'report/img/resultSquare2.png');
%h = velfind(laser1, laser2, fs);
title('2');

