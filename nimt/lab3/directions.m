close all;

fs = 100000;

%% Measurements
fid = fopen('DMeasurements/Dwithout_full.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
h = velfind(laser1, laser2, fs);
% saveas(h, 'report/img/C1magnet1skrew_full.png');
title('Dwithout_full.lvm');

fid = fopen('DMeasurements/Dsouth3S1M_full.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
h = velfind(laser1, laser2, fs);
% saveas(h, 'report/img/C1magnet1skrew_full.png');
title('Dsouth3S1M_full.lvm');

fid = fopen('DMeasurements/Dwest3S1M_full.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
h = velfind(laser1, laser2, fs);
% saveas(h, 'report/img/C1magnet1skrew_full.png');
title('Dwest3S1M_full.lvm');

fid = fopen('DMeasurements/Deast3S1M_full.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
h = velfind(laser1, laser2, fs);
% saveas(h, 'report/img/C1magnet1skrew_full.png');
title('Deast3S1M_full.lvm');

fid = fopen('DMeasurements/Dnorth3S1M_full.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
h = velfind(laser1, laser2, fs);
% saveas(h, 'report/img/C1magnet1skrew_full.png');
title('Dnorth3S1M_full.lvm');