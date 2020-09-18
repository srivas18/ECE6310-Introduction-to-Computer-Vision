data = xlsread('Outputs.csv');
for i=1:length(data)
    data(i,6) = ((data(i,4)-1).^2 + data(i,5).^2).^0.5;
end
[val, idx] = min(data(:,6));
tp = sprintf("Optimal T = %d\nTP = %d\nFP = %d\nTPR = %.4f\nFPR = %.4f",data(idx,1),data(idx,2),data(idx,3),data(idx,4),data(idx,5));
figure(1)
plot(data(:,5),data(:,4),'k','LineWidth',1.5)
grid on
hold on
xlabel('FPR'); ylabel('TPR');
plot(data(idx,5), data(idx,4), 'ro', 'MarkerFaceColor', 'r')
text(data(idx,5), (data(idx,4)-0.12), tp)
