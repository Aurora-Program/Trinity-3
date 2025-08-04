#!/usr/bin/env python3
"""
Aurora Trinity-3 - Final Deployment to Hugging Face
==================================================
"""

def check_ready_for_hf():
    """Check if Aurora is ready for HF deployment."""
    print("🔍 Checking Aurora Trinity-3 readiness for Hugging Face...")
    print("=" * 60)
    
    # Test core functionality
    try:
        from trinity_3 import Trigate, FractalTensor, get_model_info
        
        # Basic functionality test
        trigate = Trigate()
        result = trigate.infer([0, 1, 0], [1, 0, 1], [1, 1, 0])
        
        info = get_model_info()
        print(f"✅ Aurora Core: {info['name']} v{info['version']}")
        print(f"✅ Trigate Test: {result}")
        
        # Test fractal tensor
        tensor = FractalTensor(nivel_3=[[1, 0, 1]])
        print(f"✅ FractalTensor: {tensor.nivel_3[0]}")
        
    except Exception as e:
        print(f"❌ Core test failed: {e}")
        return False
    
    # Check required files
    import os
    required_files = {
        "README_HF.md": "Main model documentation",
        "config.json": "Model configuration",
        "trinity_3/core.py": "Core Aurora implementation", 
        "trinity_3/__init__.py": "Package initialization",
        "setup_clean.py": "Clean setup configuration",
        "demo_publicacion.py": "Publication demo"
    }
    
    print(f"\n📁 Required Files:")
    all_files_present = True
    for file_path, description in required_files.items():
        if os.path.exists(file_path):
            size = os.path.getsize(file_path) / 1024
            print(f"✅ {file_path} ({size:.1f}KB) - {description}")
        else:
            print(f"❌ {file_path} - MISSING - {description}")
            all_files_present = False
    
    return all_files_present

def create_hf_package():
    """Create deployment package for Hugging Face."""
    import os
    import shutil
    
    print(f"\n📦 Creating Hugging Face deployment package...")
    
    # Create clean deployment directory
    deploy_dir = "aurora_hf_ready"
    if os.path.exists(deploy_dir):
        shutil.rmtree(deploy_dir)
    os.makedirs(deploy_dir)
    
    # File mapping for HF deployment
    file_mappings = {
        "README_HF.md": "README.md",  # HF expects README.md as main doc
        "config.json": "config.json",
        "setup_clean.py": "setup.py"
    }
    
    # Copy individual files
    for src, dst in file_mappings.items():
        if os.path.exists(src):
            shutil.copy2(src, os.path.join(deploy_dir, dst))
            print(f"✅ {src} → {dst}")
    
    # Copy entire trinity_3 directory
    if os.path.exists("trinity_3"):
        shutil.copytree("trinity_3", os.path.join(deploy_dir, "trinity_3"))
        print(f"✅ trinity_3/ → trinity_3/")
    
    # Create requirements.txt (optional dependencies)
    requirements_content = """# Aurora Trinity-3 - Optional Dependencies
# Core Aurora works without any dependencies (pure Python)
# These are only needed for advanced features:

# For Hugging Face integration (optional):
# torch>=2.0.0
# transformers>=4.30.0
# huggingface-hub>=0.16.0

# For development (optional):
# pytest>=6.0.0
# black>=22.0.0
"""
    
    with open(os.path.join(deploy_dir, "requirements.txt"), "w") as f:
        f.write(requirements_content)
    print(f"✅ requirements.txt created")
    
    print(f"\n📁 Package ready in: {deploy_dir}/")
    return deploy_dir

def show_upload_instructions(deploy_dir):
    """Show manual upload instructions."""
    print(f"\n🚀 Hugging Face Upload Instructions:")
    print("=" * 60)
    
    print("1. Install Hugging Face CLI:")
    print("   pip install huggingface_hub")
    print()
    
    print("2. Login to Hugging Face:")
    print("   huggingface-cli login")
    print("   (Enter your HF token when prompted)")
    print()
    
    print("3. Create repository on Hugging Face website:")
    print("   - Go to: https://huggingface.co/new")
    print("   - Repository type: Model")
    print("   - Repository name: aurora-trinity-3")
    print("   - Visibility: Public")
    print("   - License: Apache 2.0")
    print()
    
    print("4. Clone your new repository:")
    print("   git clone https://huggingface.co/YOUR-USERNAME/aurora-trinity-3")
    print("   cd aurora-trinity-3")
    print()
    
    print(f"5. Copy files from deployment package:")
    print(f"   cp -r ../{deploy_dir}/* .")
    print("   (or manually copy all files from the deployment folder)")
    print()
    
    print("6. Upload to Hugging Face:")
    print("   git add .")
    print('   git commit -m "Add Aurora Trinity-3 v1.0.0"')
    print("   git push")
    print()
    
    print("7. Test your uploaded model:")
    print("   Visit: https://huggingface.co/YOUR-USERNAME/aurora-trinity-3")
    print()
    
    print("8. Use your model:")
    print("   pip install git+https://huggingface.co/YOUR-USERNAME/aurora-trinity-3")
    print("   from aurora_trinity import Trigate")

def main():
    print("🌌 Aurora Trinity-3 - Hugging Face Deployment")
    print("=" * 60)
    
    # Check readiness
    if not check_ready_for_hf():
        print("\n❌ Aurora is not ready for deployment. Please fix the issues above.")
        return
    
    print("\n✅ Aurora Trinity-3 is ready for Hugging Face!")
    
    # Create deployment package
    deploy_dir = create_hf_package()
    
    # Show instructions
    show_upload_instructions(deploy_dir)
    
    print(f"\n🎉 Aurora Trinity-3 deployment package ready!")
    print(f"📦 Location: {deploy_dir}/")
    print(f"🚀 Follow the upload instructions above to publish to Hugging Face")
    print(f"🌟 Aurora will be available as: huggingface.co/YOUR-USERNAME/aurora-trinity-3")

if __name__ == "__main__":
    main()
