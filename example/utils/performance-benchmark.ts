// src/utils/types.ts
export type BenchmarkResult = {
	operationName: string;
	executionTime: number;
	iterations: number;
	averageTime: number;
	minTime: number;
	maxTime: number;
	errors: number; // エラー回数を追加
};

export interface IBenchmarkUtil {
	measure<T>(
		operationName: string,
		operation: () => T | Promise<T>,
		iterations?: number,
	): Promise<BenchmarkResult>;
	getResults(operationName?: string): BenchmarkResult[];
	clearResults(operationName?: string): void;
}

// エラー型の定義
export class BenchmarkError extends Error {
	constructor(
		message: string,
		public readonly originalError?: unknown,
	) {
		super(message);
		this.name = "BenchmarkError";
	}
}
export class BenchmarkUtil implements IBenchmarkUtil {
	private static instance: BenchmarkUtil;
	private results: Map<string, number[]> = new Map();

	private constructor() {}

	static getInstance(): BenchmarkUtil {
		if (!BenchmarkUtil.instance) {
			BenchmarkUtil.instance = new BenchmarkUtil();
		}
		return BenchmarkUtil.instance;
	}

	private async executeOperation<T>(
		operation: () => T | Promise<T>,
	): Promise<{ executionTime: number; error?: Error }> {
		const startTime = performance.now();

		try {
			await Promise.resolve(operation());
			const endTime = performance.now();
			return { executionTime: endTime - startTime };
		} catch (error) {
			const endTime = performance.now();
			return {
				executionTime: endTime - startTime,
				error: error instanceof Error ? error : new Error(String(error)),
			};
		}
	}

	async measure<T>(
		operationName: string,
		operation: () => T | Promise<T>,
		iterations = 1,
	): Promise<BenchmarkResult> {
		if (iterations < 1) {
			throw new BenchmarkError("Iterations must be greater than 0");
		}

		const times: number[] = [];
		let errorCount = 0;

		try {
			for (let i = 0; i < iterations; i++) {
				const { executionTime, error } = await this.executeOperation(operation);

				if (error) {
					errorCount++;
					console.error(`Error in iteration ${i + 1}:`, error);
				}

				times.push(executionTime);
			}

			const existingTimes = this.results.get(operationName) || [];
			this.results.set(operationName, [...existingTimes, ...times]);

			const totalTime = times.reduce((acc, time) => acc + time, 0);
			const averageTime = totalTime / iterations;
			const minTime = Math.min(...times);
			const maxTime = Math.max(...times);

			return {
				operationName,
				executionTime: totalTime,
				iterations,
				averageTime,
				minTime,
				maxTime,
				errors: errorCount,
			};
		} catch (error) {
			throw new BenchmarkError(
				`Failed to complete benchmark for ${operationName}`,
				error,
			);
		}
	}

	getResults(operationName?: string): BenchmarkResult[] {
		if (operationName) {
			const times = this.results.get(operationName);
			if (!times) return [];

			const totalTime = times.reduce((acc, time) => acc + time, 0);
			return [
				{
					operationName,
					executionTime: totalTime,
					iterations: times.length,
					averageTime: totalTime / times.length,
					minTime: Math.min(...times),
					maxTime: Math.max(...times),
					errors: 0,
				},
			];
		}

		return Array.from(this.results.entries()).map(([name, times]) => {
			const totalTime = times.reduce((acc, time) => acc + time, 0);
			return {
				operationName: name,
				executionTime: totalTime,
				iterations: times.length,
				averageTime: totalTime / times.length,
				minTime: Math.min(...times),
				maxTime: Math.max(...times),
				errors: 0,
			};
		});
	}

	clearResults(operationName?: string): void {
		if (operationName) {
			this.results.delete(operationName);
		} else {
			this.results.clear();
		}
	}
}

// シングルトンインスタンスをエクスポート
export const benchmark = BenchmarkUtil.getInstance();
