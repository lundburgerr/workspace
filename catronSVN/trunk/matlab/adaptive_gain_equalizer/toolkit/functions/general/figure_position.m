function [x_pos, y_pos, fg_width, fg_heigth] = figure_position(X_nr, Y_nr)
%FIGURE_POSITION - Creates a grid of size (X_nr * Y_nr) of figure positions
% according to the PC screen size. The width of each figure is fg_width and
% the heigth is fg_heigth.
%
% Syntax:  [x_pos, y_pos, fg_width, fg_heigth] = figure_position(X_nr, Y_nr)
%
% Inputs:
%    X_nr - Nr of figure columns
%    Y_nr - Nr of figure rows
%
% Outputs:
%    x_pos - Starting positions for each column of figures
%    y_pos - Starting positions for each row of figures
%    fg_width - Width of each column
%    fg_heigth - Heigth of each row
%
% Example: 
%	% Number of active figures
%	nFig = gcf;
%	X_nr = 4;
%	Y_nr = ceil(nFig/X_nr);
%	[x_pos, y_pos, fg_width, fg_heigth]=figure_position(X_nr, Y_nr);
%
%	fignr = 1; % Init
%	for m=1:Y_nr
%		for n=1:X_nr
%			set(figure(fignr),'Position',[x_pos(n) y_pos(m) fg_width fg_heigth]);
%			% Increase figure nr
%			fignr = fignr + 1;
%		end
%	end
%	% Close non-active figures
%	for(i=nFig+1:(X_nr*Y_nr))
%		close(i)
%	end
%
%
% Other m-files required: none
% Subfunctions: none
% MAT-files required: none

% Author - Markus Borgh
% E-mail - markus.borgh@limesaudio.com
% Website - www.limesaudio.com
% Copyright - Limes Audio AB
% Created - 2009-05-26
% Last modification - 2010-10-21

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
