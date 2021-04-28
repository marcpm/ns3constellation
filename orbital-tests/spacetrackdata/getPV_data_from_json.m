
function [Pxyz, Vxyz] = getPV_data_from_json(filename)
% %  Output: Pxyz, Vxyz -- shape: [n_satellites x n_timesteps x 3]
% % % % % % % % % % % % % % % % 
debug_plotting = true;
% filename = 'iridium_TLES_PV.json';

data = jsondecode(fileread(filename));
datafields = fieldnames(data); % hay un datafield por cada satelite

ntimesteps = length(data.(string(datafields(1))).P);

Pxyz = zeros(length(datafields), ntimesteps, 3);
Vxyz = zeros(length(datafields), ntimesteps, 3);

for i = 1:length(datafields)
    
    xnoradID =   string(datafields(i));
    
    Pvec = data.(xnoradID).P;
    Vvec = data.(xnoradID).V;
%     for each sat / xnoradID


    Pxyz(i,:,:) = Pvec;
    Vxyz(i,:,:) = Vvec;
    
end


if debug_plotting
% % % % %  plot  Cutre  de prueba %%%%%%%
    figure
    ellipsoid(0,0,0, 6372e3, 6372e3, 6372e3)
    hold on
    axis equal

    for i = 1:length(datafields)
    scatter3(Pxyz(i,:,1), Pxyz(i,:,2), Pxyz(i,:,3),  'filled')
    hold on
    end
hold off

end