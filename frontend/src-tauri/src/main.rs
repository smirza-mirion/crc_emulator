#![cfg_attr(
    all(not(debug_assertions), target_os = "windows"),
    windows_subsystem = "windows"
)]

use std::process::{Child, Command};
use std::sync::Mutex;
use tauri::Manager;

struct FirmwareProcess(Mutex<Option<Child>>);

fn find_firmware_binary(app: &tauri::AppHandle) -> Option<std::path::PathBuf> {
    // Look for the firmware binary in several locations
    let candidates = vec![
        // Development: build directory relative to frontend
        app.path_resolver()
            .resolve_resource("../build/crc_emulator")
            .unwrap_or_default(),
        // Bundled: resources directory
        app.path_resolver()
            .resolve_resource("firmware/crc_emulator")
            .unwrap_or_default(),
        // Local directory
        std::path::PathBuf::from("./crc_emulator"),
        std::path::PathBuf::from("../build/crc_emulator"),
    ];

    for path in candidates {
        if path.exists() {
            return Some(path);
        }
    }
    None
}

fn main() {
    tauri::Builder::default()
        .setup(|app| {
            let handle = app.handle();

            // Find and launch the firmware binary
            match find_firmware_binary(&handle) {
                Some(firmware_path) => {
                    println!("Launching firmware: {:?}", firmware_path);

                    let child = Command::new(&firmware_path)
                        .arg("--ws-port=9876")
                        .arg("--scenario=0")
                        .spawn();

                    match child {
                        Ok(process) => {
                            println!("Firmware process started (PID: {})", process.id());
                            app.manage(FirmwareProcess(Mutex::new(Some(process))));
                        }
                        Err(e) => {
                            eprintln!("Failed to start firmware: {}", e);
                            eprintln!("The emulator will run in frontend-only mode.");
                            eprintln!("Start the firmware manually: {:?} --ws-port=9876", firmware_path);
                            app.manage(FirmwareProcess(Mutex::new(None)));
                        }
                    }
                }
                None => {
                    eprintln!("Firmware binary not found!");
                    eprintln!("Build it first: cd .. && cmake -B build && cmake --build build");
                    eprintln!("The emulator will run in frontend-only mode.");
                    app.manage(FirmwareProcess(Mutex::new(None)));
                }
            }

            Ok(())
        })
        .on_window_event(|event| {
            if let tauri::WindowEvent::Destroyed = event.event() {
                // Kill firmware process on window close
                let state: tauri::State<FirmwareProcess> = event.window().state();
                if let Ok(mut guard) = state.0.lock() {
                    if let Some(ref mut child) = *guard {
                        println!("Stopping firmware process...");
                        let _ = child.kill();
                        let _ = child.wait();
                        println!("Firmware process stopped.");
                    }
                }
            }
        })
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
