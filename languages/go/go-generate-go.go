
// code from: http://studygolang.com/articles/2450

//  a command that you can use to auto-generate Go code. 
// You can use it in conjunction with metaprograms like jsonenums (a library to auto-generate JSON marshalling boilerplate code for enum types) to automate the writing of tedious code. The Go standard library has a great API for parsing the AST, which makes metaprogramming tools simple and easy to write.



func main() { 
	flag.Parse() 
	if len(*typeNames) == 0 { 
		log.Fatalf("the flag -type must be set") 
	} 
	types := strings.Split(*typeNames, ",") 
 
	// Only one directory at a time can be processed, and the default is ".". 
	dir := "." 
	if args := flag.Args(); len(args) == 1 { 
		dir = args[0] 
	} else if len(args) > 1 { 
		log.Fatalf("only one directory at a time") 
	} 
 
	pkg, err := parser.ParsePackage(dir, *outputPrefix, *outputSuffix+".go") 
	if err != nil { 
		log.Fatalf("parsing package: %v", err) 
	} 
 
	var analysis = struct { 
		Command        string 
		PackageName    string 
		TypesAndValues map[string][]string 
	}{ 
		Command:        strings.Join(os.Args[1:], " "), 
		PackageName:    pkg.Name, 
		TypesAndValues: make(map[string][]string), 
	} 
 
	// Run generate for each type. 
	for _, typeName := range types { 
		values, err := pkg.ValuesOfType(typeName) 
		if err != nil { 
			log.Fatalf("finding values for type %v: %v", typeName, err) 
		} 
		analysis.TypesAndValues[typeName] = values 
 
		var buf bytes.Buffer 
		if err := generatedTmpl.Execute(&buf, analysis); err != nil { 
			log.Fatalf("generating code: %v", err) 
		} 
 
		src, err := format.Source(buf.Bytes()) 
		if err != nil { 
			// Should never happen, but can arise when developing this code. 
			// The user can compile the output to see the error. 
			log.Printf("warning: internal error: invalid Go generated: %s", err) 
			log.Printf("warning: compile the package to analyze the error") 
			src = buf.Bytes() 
		} 
 
		output := strings.ToLower(*outputPrefix + typeName + 
			*outputSuffix + ".go") 
		outputPath := filepath.Join(dir, output) 
		if err := ioutil.WriteFile(outputPath, src, 0644); err != nil { 
			log.Fatalf("writing output: %s", err) 
		} 
	} 
} 

