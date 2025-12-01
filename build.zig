const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const wayland_protocols = b.run(&.{
        "pkg-config",
        "--variable=pkgdatadir",
        "wayland-protocols",
    });

    const mkdir_protocols = b.addSystemCommand(&.{ "mkdir", "-p", "protocols" });

    const gen_header = b.addSystemCommand(&.{
        "wayland-scanner",
        "server-header",
        b.fmt("{s}/stable/xdg-shell/xdg-shell.xml", .{std.mem.trim(u8, wayland_protocols, " \n\r\t")}),
        "protocols/xdg-shell-protocol.h",
    });
    gen_header.step.dependOn(&mkdir_protocols.step);

    const gen_code = b.addSystemCommand(&.{
        "wayland-scanner",
        "private-code",
        b.fmt("{s}/stable/xdg-shell/xdg-shell.xml", .{std.mem.trim(u8, wayland_protocols, " \n\r\t")}),
        "protocols/xdg-shell-protocol.c",
    });
    gen_code.step.dependOn(&mkdir_protocols.step);

    const root_module = b.createModule(.{
        .target = target,
        .optimize = optimize,
    });

    const exe = b.addExecutable(.{
        .name = "dwc",
        .root_module = root_module,
    });

    exe.addCSourceFiles(.{
        .files = &.{
            "src/main.c",
            "src/server.c",
            "src/xdg_shell.c",
            "src/input.c",
            "src/output.c",
            "src/layout.c",
            "protocols/xdg-shell-protocol.c",
        },
        .flags = &.{
            "-std=c11",
            "-Wall",
            "-Wextra",
            "-pedantic",
            "-Wno-unused-parameter",
            "-DWLR_USE_UNSTABLE",
            "-D_POSIX_C_SOURCE=200809L",
            "-DVERSION=\"0.1.0\"",
        },
    });

    exe.addIncludePath(b.path("include"));
    exe.addIncludePath(b.path("protocols"));

    exe.linkLibC();
    exe.linkSystemLibrary("wlroots-0.19");
    exe.linkSystemLibrary("wayland-server");
    exe.linkSystemLibrary("xkbcommon");
    exe.linkSystemLibrary("pixman-1");

    exe.step.dependOn(&gen_header.step);
    exe.step.dependOn(&gen_code.step);

    b.installArtifact(exe);

    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());
    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the compositor");
    run_step.dependOn(&run_cmd.step);

    const clean_step = b.step("clean", "Remove build artifacts");
    const clean_cmd = b.addSystemCommand(&.{
        "rm", "-rf", "zig-out", "zig-cache", "protocols",
    });
    clean_step.dependOn(&clean_cmd.step);

    const test_cmd = b.addRunArtifact(exe);
    test_cmd.step.dependOn(b.getInstallStep());
    if (b.args) |args| {
        test_cmd.addArgs(args);
    }

    const test_step = b.step("test", "Test the compositor");
    test_step.dependOn(&test_cmd.step);
}
