clear all; close all; clc;
filename = 'testOutput.out';
fprintf('Reading file %s...\n',filename);

f = fopen(filename,'r');

head = fgetl(f);

disp(head);
[n,steps,dt] = sscanf(head,'SIMULATION %i BODIES, %i STEPS, %d DT');


% x y z vx vy vz columns
data = fscanf(f,'%f %f %f %f %f %f', [6 inf])';

figure;
hold on;
colors = 'bgryk';
getColor = @(x) colors(mod(x,length(colors)));
for i = 1:n
    plot(data(i:n:end,1),data(i:n:end,2),  sprintf('.-%c',getColor(i))  );
end
hold off;
axis equal;
fclose(f);