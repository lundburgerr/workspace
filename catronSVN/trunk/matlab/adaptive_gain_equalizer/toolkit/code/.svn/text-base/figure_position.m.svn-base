function [x_pos, y_pos, fg_width, fg_heigth] = figure_position(X_nr, Y_nr)
% [x_pos, y_pos, fg_width, fg_heigth] = figure_position(X_nr, Y_nr)
% Creates a grid of size (X_nr * Y_nr) of figure positions according to the
% screen size. The width of each figure is fg_width and the heigth is
% fg_heigth. When creating a new figure, use the command
% >> figure('Position',[x_pos(m) y_pos(n) fg_width fg_heigth]);
% or 
% >> set(figure(x),'Position',[x_pos(m) y_pos(n) fg_width fg_heigth])
%
% Author - Markus Borgh
% Date - 2009-05-26
% Copyright - Limes Technology

% ------ Plot settings ------
% Placement of figures
scrsz = get(0,'ScreenSize'); % Screensize

% Width and heigth of figures
fg_width = scrsz(3)/X_nr*0.9;
fg_heigth = scrsz(4)/(Y_nr+1.2);

% Position of lower left corner of figure is given by [x_pos(m), y_pos(n)]
x_pos = ([0:X_nr-1]./X_nr + 10/scrsz(3)) .* scrsz(3);
y_pos = ([Y_nr-1:-1:0]./Y_nr + 0) .* scrsz(4);

% Increase the lower row (so that the start menu is out of the way)
y_pos(Y_nr) = y_pos(Y_nr) + 0.07*scrsz(4);
