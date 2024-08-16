import numpy as np

def resample(data, tgt_rate, rate):
    """
    Resamples the input signal data from the original rate to the target rate.

    Parameters:
    data (np.ndarray): The input signal data as a NumPy array.
    tgt_rate (int): The target sampling rate (in Hz).
    rate (int): The original sampling rate (in Hz).

    Returns:
    np.ndarray: The resampled data.
    """
    # Calculate the resampling factor
    resample_factor = tgt_rate / rate
    
    # Determine the number of samples in the resampled data
    num_samples = int(len(data) * resample_factor)
    
    # Use numpy's interpolation function to resample the data
    resampled_data = np.interp(
        np.linspace(0, len(data), num_samples, endpoint=False),
        np.arange(len(data)),
        data
    )

    return resampled_data

if __name__ == "__main__":
    # Example usage
    data = np.array([10.0 for _ in range(100)], dtype=np.double)
    tgt_rate = 10
    rate = 100

    result = resample(data, tgt_rate, rate)
    # print(result)

    data = np.random.rand(100)
    resampled_data = resample(data, tgt_rate, rate)

    import matplotlib.pyplot as plt

    plt.plot(data)
    plt.show()

    plt.plot(resampled_data)
    plt.show()