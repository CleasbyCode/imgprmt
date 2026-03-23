#include "imgprmt_io.hpp"

#if defined(__has_include)
#if __has_include(<linux/openat2.h>)
#include <linux/openat2.h>
#define IMGPRMT_IO_HAS_OPENAT2 1
#endif
#endif

#include <sys/resource.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <format>
#include <iostream>
#include <limits>
#include <print>
#include <random>
#include <stdexcept>
#include <string_view>
#include <utility>

namespace imgprmt::io {
output_file::output_file() = default;

output_file::output_file(int fd_, std::string path_) : fd(fd_), path(std::move(path_)) {}

output_file::output_file(output_file&& other) noexcept : fd(other.fd), path(std::move(other.path)) {
	other.fd = -1;
}

output_file& output_file::operator=(output_file&& other) noexcept {
	if (this != &other) {
		reset();
		fd = other.fd;
		path = std::move(other.path);
		other.fd = -1;
	}
	return *this;
}

output_file::~output_file() {
	reset();
}

void output_file::reset() noexcept {
	if (fd >= 0) {
		::close(fd);
		fd = -1;
	}
}

input_file::input_file() = default;

input_file::input_file(int fd_) : fd(fd_) {}

input_file::input_file(input_file&& other) noexcept : fd(other.fd) {
	other.fd = -1;
}

input_file& input_file::operator=(input_file&& other) noexcept {
	if (this != &other) {
		reset();
		fd = other.fd;
		other.fd = -1;
	}
	return *this;
}

input_file::~input_file() {
	reset();
}

void input_file::reset() noexcept {
	if (fd >= 0) {
		::close(fd);
		fd = -1;
	}
}

output_file create_output_file() {
	std::random_device rd;
	const std::uint64_t seed = (static_cast<std::uint64_t>(rd()) << 32) ^ static_cast<std::uint64_t>(rd());
	std::mt19937_64 gen(seed);
	std::uniform_int_distribution<unsigned int> dist(10000, 99999);

	for (int attempt = 0; attempt < 256; ++attempt) {
		std::string candidate = std::format("imgprmt_{}.jpg", dist(gen));
		int fd = ::open(candidate.c_str(), O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC | O_NOFOLLOW, 0644);
		if (fd >= 0) {
			return output_file{fd, std::move(candidate)};
		}

		if (errno != EEXIST) {
			throw std::runtime_error(std::format("Write Error: Unable to create output file ({}).", std::strerror(errno)));
		}
	}

	throw std::runtime_error("Write Error: Unable to allocate a unique output filename.");
}

void write_all_fd(int fd, std::span<const std::uint8_t> data) {
	std::size_t offset = 0;
	while (offset < data.size()) {
		ssize_t written = ::write(fd, data.data() + offset, data.size() - offset);
		if (written < 0) {
			if (errno == EINTR) {
				continue;
			}
			throw std::runtime_error(std::format("Write Error: {}", std::strerror(errno)));
		}
		if (written == 0) {
			throw std::runtime_error("Write Error: Unexpected short write.");
		}

		offset += static_cast<std::size_t>(written);
	}
}

void read_all_fd(int fd, std::span<std::uint8_t> data) {
	std::size_t offset = 0;
	while (offset < data.size()) {
		ssize_t bytes_read = ::read(fd, data.data() + offset, data.size() - offset);
		if (bytes_read < 0) {
			if (errno == EINTR) {
				continue;
			}
			throw std::runtime_error(std::format("Read File Error: {}", std::strerror(errno)));
		}
		if (bytes_read == 0) {
			throw std::runtime_error("Read File Error: Unexpected end of file.");
		}

		offset += static_cast<std::size_t>(bytes_read);
	}
}

int open_input_readonly_secure(const std::filesystem::path& path) {
	constexpr int FALLBACK_FLAGS = O_RDONLY | O_CLOEXEC | O_NOFOLLOW;
#if defined(IMGPRMT_IO_HAS_OPENAT2) && defined(SYS_openat2)
	std::uint64_t resolve_flags = RESOLVE_NO_SYMLINKS;
#ifdef RESOLVE_NO_MAGICLINKS
	resolve_flags |= RESOLVE_NO_MAGICLINKS;
#endif
	if (!path.is_absolute()) {
		resolve_flags |= RESOLVE_BENEATH;
	}

	open_how how{};
	how.flags = O_RDONLY | O_CLOEXEC;
	how.resolve = resolve_flags;

	int fd = static_cast<int>(::syscall(SYS_openat2, AT_FDCWD, path.c_str(), &how, sizeof(how)));
	if (fd >= 0) {
		return fd;
	}

	if (errno != ENOSYS && errno != EINVAL && errno != E2BIG) {
		return -1;
	}
#endif
	return ::open(path.c_str(), FALLBACK_FLAGS);
}

void apply_runtime_limits() {
	if (const char* disable = std::getenv("IMGPRMT_DISABLE_LIMITS");
	    disable && std::string_view(disable) == "1") {
		return;
	}

	const rlimit cpu_limit{
		static_cast<rlim_t>(20),
		static_cast<rlim_t>(25),
	};
	if (::setrlimit(RLIMIT_CPU, &cpu_limit) != 0) {
		std::println(std::cerr, "Warning: Unable to set CPU runtime limit ({}).", std::strerror(errno));
	}

	constexpr std::uint64_t AS_LIMIT_BYTES = 1024ULL * 1024ULL * 1024ULL;
	const auto as_limit_value_u64 = std::min<std::uint64_t>(
		AS_LIMIT_BYTES,
		static_cast<std::uint64_t>(std::numeric_limits<rlim_t>::max()));
	const rlimit as_limit{
		static_cast<rlim_t>(as_limit_value_u64),
		static_cast<rlim_t>(as_limit_value_u64),
	};
	if (::setrlimit(RLIMIT_AS, &as_limit) != 0) {
		std::println(std::cerr, "Warning: Unable to set address-space limit ({}).", std::strerror(errno));
	}
}
}  // namespace imgprmt::io
