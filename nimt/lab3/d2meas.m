close all;

fs = 100000;

%% Measurements
fid = fopen('D2Measurements/Dadded2skrew_full.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
h = velfind(laser1, laser2, fs);
% saveas(h, 'report/img/C1magnet1skrew_full.png');
title('2skrew');

fid = fopen('D2Measurements/Dadded4skrew_full.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
h = velfind(laser1, laser2, fs);
% saveas(h, 'report/img/C1magnet1skrew_full.png');
title('4skrew');

fid = fopen('D2Measurements/Dadded6skrew_full.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
h = velfind(laser1, laser2, fs);
% saveas(h, 'report/img/C1magnet1skrew_full.png');
title('6skrew');

fid = fopen('D2Measurements/Dadded8skrew_full.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
h = velfind(laser1, laser2, fs);
% saveas(h, 'report/img/C1magnet1skrew_full.png');
title('8skrew');

fid = fopen('D2Measurements/Dadded10skrew_full.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
h = velfind(laser1, laser2, fs);
% saveas(h, 'report/img/C1magnet1skrew_full.png');
title('10skrew');

