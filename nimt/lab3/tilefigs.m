function tilefigs(figs)
%TILEFIGS Tile all open figure windows around on the screen.
% TILEFIGS places all open figure windows around on the screen with no
% overlap. TILEFIGS(FIGS) can be used to specify which figures that
% should be tiled. Figures are not sorted when specified.
%
% 1998-01-22 19:40:40 Peter J. Acklam <jacklam@math.uio.no>

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Get the handles to the figures to process.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

if ( nargin == 0 ) % If no input arguments...
   figs = findobj( 'Type', 'figure' ); % ...find all figures.
   figs = sort( figs );
end

if isempty( figs )
   disp( 'No open figures or no figures specified.' );
   return;
end

nfigs = length( figs );

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Set miscellaneous parameter.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

nfigs = length(figs); % Number of figures.

nh = ceil( sqrt(nfigs) ); % Number of figures horisontally.
nv = ceil( nfigs/nh ); % Number of figures vertically.

nh = max( nh, 2 );
nv = max( nv, 2 );

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Get the screen size.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

set( 0, 'Units', 'pixels' ); % Set root units.
scrdim = get( 0, 'ScreenSize' ); % Get screen size.
scrwid = scrdim(3); % Screen width.
scrhgt = scrdim(4); % Screen height.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The elements in the vector specifying the position.
% 1 - Window left position
% 2 - Window bottom position
% 3 - Window horizontal size
% 4 - Window vertical size
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

hspc = 50; % Horisontal space.
topspc = 80; % Space above top figure.
medspc = 80; % Space between figures.
botspc = 40; % Space below bottom figure.

figwid = ( scrwid - (nh+1)*hspc )/nh;
fighgt = ( scrhgt - (topspc+botspc) - (nv-1)*medspc )/nv;

figwid = round(figwid);
fighgt = round(fighgt);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Put the figures where they belong.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

for row = 1:nv
   for col = 1:nh
      idx = (row-1)*nh + col;
      if ( idx <= nfigs )
         figlft = col*hspc + (col-1)*figwid;
         figbot = scrhgt - topspc - row*fighgt - (row-1)*medspc;
         figpos = [ figlft figbot figwid fighgt ]; % Figure position.
         fighnd = figs(idx); % Figure handle.
         units = get( fighnd, 'Units' ); % Get units.
         set( fighnd, 'Units', 'pixels' ); % Set new units.
         set( fighnd, 'Position', figpos ); % Set position.
         figure( fighnd ); % Raise figure.
      end
   end
end