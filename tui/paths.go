package main

import (
	"os"
	"path/filepath"
	"strings"
)

func getProjectRoot() string {

	markers := []string{
		"Makefile",
		"go.mod",
		"README.md",
		"data",
		"examples",
	}

	exePath, err := os.Executable()
	if err != nil {

		cwd, _ := os.Getwd()
		return findProjectRoot(cwd, markers)
	}

	if strings.Contains(exePath, "go-build") || strings.Contains(exePath, os.TempDir()) {
		cwd, _ := os.Getwd()
		return findProjectRoot(cwd, markers)
	}

	exeDir := filepath.Dir(exePath)
	return findProjectRoot(exeDir, markers)
}

func findProjectRoot(startDir string, markers []string) string {
	currentDir := startDir

	for i := 0; i < 10; i++ {

		hasMarkers := 0
		for _, marker := range markers {
			path := filepath.Join(currentDir, marker)
			if _, err := os.Stat(path); err == nil {
				hasMarkers++
			}
		}

		if hasMarkers >= 3 {
			return currentDir
		}

		parentDir := filepath.Dir(currentDir)
		if parentDir == currentDir {

			break
		}
		currentDir = parentDir
	}

	return startDir
}

func getAbsolutePath(relativePath string) string {
	root := getProjectRoot()
	absPath := filepath.Join(root, relativePath)

	if _, err := os.Stat(absPath); err == nil {
		return absPath
	}

	return absPath
}

func findFile(possiblePaths []string) string {
	for _, path := range possiblePaths {
		absPath := getAbsolutePath(path)
		if _, err := os.Stat(absPath); err == nil {
			return absPath
		}
	}
	return ""
}

func getDefaultMicrocodePath() string {
	possiblePaths := []string{
		"data/basic_microcode.txt",
		"../data/basic_microcode.txt",
		"./data/basic_microcode.txt",
	}

	path := findFile(possiblePaths)
	if path != "" {
		return path
	}

	return getAbsolutePath("data/basic_microcode.txt")
}

func getDefaultExamplePath() string {
	possiblePaths := []string{
		"examples/sum.asm",
		"../examples/sum.asm",
		"./examples/sum.asm",
	}

	path := findFile(possiblePaths)
	if path != "" {
		return path
	}

	return getAbsolutePath("examples/sum.asm")
}

func findAsmFiles() []string {
	possibleDirs := []string{
		"examples",
		"../examples",
		"./examples",
	}

	var asmFiles []string

	for _, dir := range possibleDirs {
		absDir := getAbsolutePath(dir)
		entries, err := os.ReadDir(absDir)
		if err != nil {
			continue
		}

		for _, entry := range entries {
			if !entry.IsDir() && filepath.Ext(entry.Name()) == ".asm" {
				asmFiles = append(asmFiles, entry.Name())
			}
		}

		if len(asmFiles) > 0 {
			return asmFiles
		}
	}

	return asmFiles
}

func getAsmFilePath(filename string) string {
	possiblePaths := []string{
		filepath.Join("examples", filename),
		filepath.Join("../examples", filename),
		filepath.Join("./examples", filename),
	}

	return findFile(possiblePaths)
}
