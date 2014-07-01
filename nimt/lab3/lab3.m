close all;

fs = 100000;

%% Just rod and stuff
fid = fopen('CMeasurements/C1magnet1skrew_full.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
h = velfind(laser1, laser2, fs);
saveas(h, 'report/img/C1magnet1skrew_full.png');
title('C1magnet1skrew_full.lvm');


fid = fopen('CMeasurements/C1magnet2skrew_full.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
h = velfind(laser1, laser2, fs);
saveas(h, 'report/img/C1magnet2skrew_full.png');
title('C1magnet2skrew_full.lvm');


fid = fopen('CMeasurements/C1magnet3skrew_full.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
h = velfind(laser1, laser2, fs);
saveas(h, 'report/img/C1magnet3skrew_full.png');
title('C1magnet3skrew_full.lvm');


fid = fopen('CMeasurements/C2magnet2skrew_full.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
h = velfind(laser1, laser2, fs);
saveas(h, 'report/img/C2magnet2skrew_full.png');
title('C2magnet2skrew_full.lvm');


fid = fopen('CMeasurements/C2magnet4skrew_full.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
h = velfind(laser1, laser2, fs);
saveas(h, 'report/img/C2magnet4skrew_full.png');
title('C2magnet4skrew_full.lvm');


fid = fopen('CMeasurements/C2magnet6skrew_full.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
h = velfind(laser1, laser2, fs);
saveas(h, 'report/img/C2magnet6skrew_full.png');
title('C2magnet6skrew_full.lvm');

%% Cardboard 
fid = fopen('CMeasurements/cardboard1+2_fast.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
h = velfind(laser1, laser2, fs);
saveas(h, 'report/img/cardboard1+2_fast.png');
title('cardboard1+2_fast.lvm');


fid = fopen('CMeasurements/cardboard1+2_slow.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
h = velfind(laser1, laser2, fs);
saveas(h, 'report/img/cardboard1+2_slow.png');
title('cardboard1+2_slow.lvm');


fid = fopen('CMeasurements/cardboard1_fast.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
h = velfind(laser1, laser2, fs);
saveas(h, 'report/img/cardboard1_fast.png');
title('cardboard1_fast.lvm');


fid = fopen('CMeasurements/cardboard1_slow.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
h = velfind(laser1, laser2, fs);
saveas(h, 'report/img/cardboard1_slow.png');
title('cardboard1_slow.lvm');


fid = fopen('CMeasurements/cardboard2_fast.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
h = velfind(laser1, laser2, fs);
saveas(h, 'report/img/cardboard2_fast.png');
title('cardboard2_fast.lvm');


fid = fopen('CMeasurements/cardboard2_slow.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
h = velfind(laser1, laser2, fs);
saveas(h, 'report/img/cardboard2_slow.png');
title('cardboard2_slow.lvm');


fid = fopen('CMeasurements/Cardboard1+2_full.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
h = velfind(laser1, laser2, fs);
saveas(h, 'report/img/Cardboard1+2_full.png');
title('Cardboard1+2_full.lvm');


fid = fopen('CMeasurements/Cardboard1_full.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
h = velfind(laser1, laser2, fs);
saveas(h, 'report/img/Cardboard1_full.png');
title('Cardboard1_full.lvm');


fid = fopen('CMeasurements/Cardboard2_full.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
h = velfind(laser1, laser2, fs);
saveas(h, 'report/img/Cardboard2_full.png');
title('Cardboard2_full.lvm');

%% Without ??
fid = fopen('CMeasurements/Cwithout_fast.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
h = velfind(laser1, laser2, fs);
saveas(h, 'report/img/Cwithout_fast.png');
title('Cwithout_fast.lvm');

fid = fopen('CMeasurements/Cwithout_full.lvm','rt');
str = fread(fid,'*char')';
str = strrep(str,',','.');
[laser1, laser2] = strread(str, '\t%f\t%f');
fclose(fid);
h = velfind(laser1, laser2, fs);
saveas(h, 'report/img/Cwithout_full.png');
title('Cwithout_full.lvm');


