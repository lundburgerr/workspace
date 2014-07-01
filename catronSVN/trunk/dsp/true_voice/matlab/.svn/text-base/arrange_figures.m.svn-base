function [] = arrange_figures(nFig, nCol)
%ARRANGE_FIGURES - Arranges figures on window in separate positions. It is
%assumed that figures are numbered 1-nFig, where nFig is the "latest"
%active figure.
%
% Syntax:  [] = arrange_figures(nFig, nCol)
%
% Inputs:
%    nFig - Number of active figures
%    nCol - Number of figure columns
%
% Outputs:
%    None
%
% Example: 
%    t = [0:0.01:1];
%    figure(1)
%    plot(t,t.^2)
%    figure(2)
%    plot(t,sin(t))
%    arrange_figures(gcf, 2); % Arrange figures in two columns
%
% Other m-files required: figure_position.m
% Subfunctions: none
% MAT-files required: none

% Author - Markus Borgh
% E-mail - markus.borgh@limesaudio.com
% Website - www.limesaudio.com
% Copyright - Limes Audio AB
% Created - 2011-Jun-08
% Last modification - 2011-Jun-08

%------------- BEGIN CODE --------------

% ---< Get figure position matrix >---
X_nr = nCol;
Y_nr = ceil(nFig/X_nr);
[x_pos, y_pos, fg_width, fg_heigth] = figure_position(X_nr, Y_nr);

% ---< Rearrange figures >---
fignr = 1; % Init
for m=1:Y_nr
    for n=1:X_nr
        set(figure(fignr),'Position',[x_pos(n) y_pos(m) fg_width fg_heigth]);
        % Increase figure nr
        fignr = fignr + 1;
    end
end
% ---< Close non-active figures >---
for(i=nFig+1:(X_nr*Y_nr))
    close(i)
end

%------------- END OF CODE --------------