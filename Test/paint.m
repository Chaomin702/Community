set(0,'defaultfigurecolor','w')
for i = 1:9
    figure;
    scatter(1:length(data{i}),data{i}(:,1),45,'g','+');
    hold on;
    scatter(1:length(data{i}),data{i}(:,2),45,'b','x');
    title(['n = ', num2str(length(data{i})*20)]);
    ylabel('diffusion time');
    xlabel('k');
    legend('community based','naive');
end