function read_benchmark_log(log)
	start_t = log(1,2);

	server_t = log(:,1) - start_t;
	ray_t    = log(:,2) - start_t;

	bench_color_conversion  = log(:,3).*1000;
	bench_thresholding      = log(:,4).*1000;
	bench_blob_detection    = log(:,5).*1000;
	bench_marker_ident      = log(:,6).*1000;
	bench_findmarkers_total = log(:,7).*1000;
	bench_client_overhead   = log(:,8).*1000;
	bench_overhead          = log(:,9).*1000;

	[mu, sigma, mu_ci, sigma_ci] = normfit(bench_color_conversion);
	fprintf('color_conversion:  %10.6f   +- %10.6f\n', mu, sigma);

	[mu, sigma, mu_ci, sigma_ci] = normfit(bench_thresholding);
	fprintf('thresholding:      %10.6f   +- %10.6f\n', mu, sigma);

	[mu, sigma, mu_ci, sigma_ci] = normfit(bench_blob_detection);
	fprintf('blob_detection:    %10.6f   +- %10.6f\n', mu, sigma);

	[mu, sigma, mu_ci, sigma_ci] = normfit(bench_marker_ident);
	fprintf('marker_ident:      %10.6f   +- %10.6f\n', mu, sigma);

	[mu, sigma, mu_ci, sigma_ci] = normfit(bench_findmarkers_total);
	fprintf('findmarkers_total: %10.6f   +- %10.6f\n', mu, sigma);

	[mu, sigma, mu_ci, sigma_ci] = normfit(bench_client_overhead);
	fprintf('client_overhead:   %10.6f   +- %10.6f\n', mu, sigma);

	[mu, sigma, mu_ci, sigma_ci] = normfit(bench_overhead);
	fprintf('overhead:          %10.6f   +- %10.6f\n', mu, sigma);

	hist(bench_marker_ident, 300)
end
